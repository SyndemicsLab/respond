////////////////////////////////////////////////////////////////////////////////
// File: TEST_DataLoader.cpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>

#include "spdlog/spdlog.h"

#include <respond/data_ops/DataLoader.hpp>

#include <datamanagement/DataManagement.hpp>

class DataLoaderTest : public ::testing::Test {
protected:
    std::filesystem::path tempRelativeFile;
    std::filesystem::path tempAbsoluteFile;
    std::filesystem::path tempRelativeFile2;
    std::filesystem::path tempAbsoluteFile2;
    std::filesystem::path configFile;
    std::ofstream fileStream;
    std::ofstream fileStream2;
    std::ofstream configFileStream;
    std::shared_ptr<spdlog::logger> logger;
    void SetUp() override {
        if (!logger) {
            if (spdlog::get("test")) {
                logger = spdlog::get("test");
            } else {
                logger = spdlog::stdout_color_mt("test");
            }
        }
        tempRelativeFile = std::tmpnam(nullptr) + std::string(".csv");
        tempAbsoluteFile =
            std::filesystem::temp_directory_path() / tempRelativeFile;

        tempRelativeFile2 = std::tmpnam(nullptr) + std::string(".csv");
        tempAbsoluteFile2 =
            std::filesystem::temp_directory_path() / tempRelativeFile2;

        configFile = std::filesystem::temp_directory_path() /
                     std::filesystem::path("sim.conf");
        configFileStream.open(configFile);

        // clang-format off
        configFileStream << "[simulation]" 
                            << std::endl << 
                            "duration = 52" 
                            << std::endl << 
                            "aging_interval = 260" 
                            << std::endl << 
                            "intervention_change_times = 52" 
                            << std::endl << 
                            "entering_sample_change_times = 52" 
                            << std::endl << 
                            "overdose_change_times = 52" 
                            << std::endl << 
                            "stratified_entering_cohort = false" 
                            << std::endl << std::endl << 
                            "[state]" 
                            << std::endl << 
                            "interventions = No_Treatment, Buprenorphine,"
                            "Naltrexone, Methadone, Detox, Post-Buprenorphine,"
                            "Post-Naltrexone, Post-Methadone, Post-Detox" ""
                            << std::endl << 
                            "ouds = Active_Noninjection, Active_Injection," 
                            "Nonactive_Noninjection, Nonactive_Injection" 
                            << std::endl << std::endl << 
                            "[demographic]" 
                            << std::endl << 
                            "age_groups = 10_14, 15_19, 20_24, 25_29, 30_34, "
                            "35_39, 40_44, 45_49, 50_54, 55_59, 60_64, 65_69, "
                            "70_74, 75_79, 80_84, 85_89, 90_94, 95_99 " 
                            << std::endl << 
                            "sex = Male, Female " 
                            << std::endl << std::endl <<
                            "[cost]" 
                            << std::endl << 
                            "cost_analysis = true" 
                            << std::endl <<  
                            "cost_perspectives = healthcare" 
                            << std::endl << 
                            "discount_rate = 0.0025 " 
                            << std::endl << 
                            "reporting_interval = 1"
                            << std::endl << 
                            "cost_utility_output_timesteps = 52 " 
                            << std::endl << 
                            "cost_category_outputs = false " 
                            << std::endl << std::endl << 
                            "[output] " 
                            << std::endl <<
                            "per_intervention_predictions = true " 
                            << std::endl <<
                            "general_outputs = false " 
                            << std::endl << 
                            "general_stats_output_timesteps = 52";
        // clang-format on
        configFileStream.close();
        fileStream.open(tempAbsoluteFile);
        fileStream2.open(tempAbsoluteFile2);
    }
    void TearDown() override {
        if (configFileStream.is_open()) {
            configFileStream.close();
        }
        if (fileStream.is_open()) {
            fileStream.close();
        }
        if (fileStream2.is_open()) {
            fileStream2.close();
        }
    }
};

TEST_F(DataLoaderTest, ConstructorEmpty) {
    data_ops::DataLoader dl;
    EXPECT_EQ(dl.getInterventions().size(), 0);
}

TEST_F(DataLoaderTest, ConstructorConfigAndStringAndLogger) {
    Data::IConfigablePtr config =
        std::make_shared<Data::Config>(configFile.string());
    data_ops::DataLoader dl(
        config, std::filesystem::temp_directory_path().string(), logger);
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, ConstructorStringAndLogger) {
    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, ConstructorConfig) {
    Data::IConfigablePtr config =
        std::make_shared<Data::Config>(configFile.string());
    data_ops::DataLoader dl(config);
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, ConstructorConfigAndString) {
    Data::IConfigablePtr config =
        std::make_shared<Data::Config>(configFile.string());
    data_ops::DataLoader dl(config,
                            std::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, ConstructorString) {
    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, initialSample) {
    fileStream
        << "block,agegrp,sex,oud,counts" << std::endl
        << "No_Treatment,10_14,Male,Active_Noninjection,2917.55795376043"
        << std::endl
        << "No_Treatment,10_14,Male,Active_Injection,977.390032367151"
        << std::endl
        << "No_Treatment,10_14,Male,Nonactive_Noninjection,288.995723856067";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadInitialSample(tempAbsoluteFile.string());

    data_ops::Matrix3d result = dl.getInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, enteringSamples) {
    fileStream << "agegrp,sex,cohort_size_change_1_52" << std::endl
               << "10_14,male,11.4389540364826" << std::endl
               << "10_14,female,7.10870959447953" << std::endl
               << "15_19,male,12.0934754686572";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadEnteringSamples(tempAbsoluteFile.string(),
                           std::string("No_Treatment"),
                           std::string("Active_Noninjection"));

    data_ops::TimedMatrix3d result = dl.getEnteringSamples();
    EXPECT_EQ(result(0, 0, 0, 0), 11.4389540364826);
}

TEST_F(DataLoaderTest, OUDTransitionRates) {
    fileStream
        << "intervention,agegrp,sex,initial_oud,Active_Noninjection,"
           "Active_"
           "Injection,Nonactive_Noninjection,Nonactive_Injection"
        << std::endl
        << "No_Treatment,10_14,male,Active_Noninjection,0.560720353446504,0."
           "406726715244475,0.0325529313090211,0"
        << std::endl
        << "No_Treatment,10_14,male,Active_Injection,0.174836047529822,0."
           "591213148061237,0,0.233950804408941"
        << std::endl
        << "No_Treatment,10_14,male,Nonactive_Noninjection,0.308436737296612,0."
           "0564816594618387,0.635081603241549,0";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadOUDTransitionRates(tempAbsoluteFile.string());

    data_ops::Matrix3d result = dl.getOUDTransitionRates();
    EXPECT_EQ(result(0, 0, 0), 0.560720353446504);
}

TEST_F(DataLoaderTest, interventionTransitionRates) {
    fileStream << "agegrp,sex,oud,initial_intervention,No_Treatment_1_52,"
                  "Buprenorphine_1_52,Naltrexone_1_52,Methadone"
                  "_1_52,"
                  "Detox_1_52,Post-Buprenorphine_1_52,Post-Naltrexone_1_52,"
                  "Post-Methadone_1_52,Post-Detox_1_52"
               << std::endl
               << "10_14,male,Active_Noninjection,No_Treatment,0."
                  "625523912484771,0.101388565684697,0.0472664681057711,0."
                  "178570136497494,0.0472509172272673,0,0,0,0"
               << std::endl
               << "10_14,male,Active_Noninjection,Buprenorphine,0,0."
                  "996990941661389,0,0,0,0.0030090583386112,0,0,0"
               << std::endl
               << "10_14,male,Active_Noninjection,Naltrexone,0,0,0."
                  "88186832069196,0,0,0,0.11813167930804,0,0";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadInterventionTransitionRates(tempAbsoluteFile.string());

    data_ops::TimedMatrix3d result = dl.getInterventionTransitionRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.625523912484771);
}

TEST_F(DataLoaderTest, overdoseRates) {
    fileStream
        << "block,agegrp,sex,oud,overdose_1_52,overdose_52_104,overdose_104_"
           "156,overdose_156_208,"
           "overdose_208_260"
        << std::endl
        << "No_Treatment,10_14,male,Active_Noninjection,0.00059346577560159,0."
           "000895542690723513,0.000780514937366773,0.000510919750042232,0."
           "000108104676301938"
        << std::endl
        << "No_Treatment,10_14,male,Active_Injection,0.00159609306875247,0."
           "00746057563493888,0.0019836509574786,0.00179252999267244,0."
           "00113800623019004"
        << std::endl
        << "No_Treatment,10_14,female,Active_Noninjection,0.000255931977117035,"
           "0.000469701882303847,0.000837585099598164,0.000557619510178251,0."
           "000348390775561181";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadOverdoseRates(tempAbsoluteFile.string());

    data_ops::TimedMatrix3d result = dl.getOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.00059346577560159);
}

TEST_F(DataLoaderTest, fatalOverdoseRates) {
    fileStream
        << "percent_overdoses_fatal_1_52,percent_overdoses_fatal_52_104,"
           "percent_overdoses_fatal_104_156,percent_overdoses_fatal_156_208,"
           "percent_overdoses_fatal_208_260"
        << std::endl
        << "0.216540329711774,0.297741215749976,0.113841797135366,0."
           "126092413319309,0.156049415151599";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadFatalOverdoseRates(tempAbsoluteFile.string());

    data_ops::TimedMatrix3d result = dl.getFatalOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.216540329711774);
}

TEST_F(DataLoaderTest, fatalOverdoseRatesStratified) {
    fileStream
        << "agegrp,sex,percent_overdoses_fatal_1_52,percent_overdoses_fatal_52_"
           "104,"
           "percent_overdoses_fatal_104_156,percent_overdoses_fatal_156_208,"
           "percent_overdoses_fatal_208_260"
        << std::endl
        << "10_14,Male,"
           "0.216540329711774,0.297741215749976,0.113841797135366,0."
           "126092413319309,0.156049415151599"
        << std::endl
        << "10_14, Female, 0.2, 0.3, 0.1, 0.126, 0.15";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadFatalOverdoseRates(tempAbsoluteFile.string());

    data_ops::TimedMatrix3d result = dl.getFatalOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.216540329711774);
    EXPECT_EQ(result(0, 1, 0, 0), 0.216540329711774);
    EXPECT_EQ(result(0, 0, 1, 1), 0.2);
    EXPECT_EQ(result(0, 1, 1, 1), 0.2);
}

TEST_F(DataLoaderTest, mortalityRates) {
    fileStream
        << "block,agegrp,sex,oud,SMR" << std::endl
        << "No_Treatment,10_14,male,Active_Noninjection,2.1203387149855"
        << std::endl
        << "No_Treatment,10_14,male,Active_Injection,5.51986056060266"
        << std::endl
        << "No_Treatment,10_14,male,Nonactive_Noninjection,1.83401409064184";
    fileStream.close();

    fileStream2 << "agegrp,sex,death_prob" << std::endl
                << "10_14,Male,3.22625844595414E-06" << std::endl
                << "10_14,Female,2.3250579153089E-06" << std::endl
                << "15_19,Male,1.2191975906517E-05";
    fileStream2.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadMortalityRates(tempAbsoluteFile.string(),
                          tempAbsoluteFile2.string());

    data_ops::Matrix3d result = dl.getMortalityRates();
    EXPECT_EQ(result(0, 0, 0), 6.8407483245769285e-06);
}

TEST_F(DataLoaderTest, interventionInitRates) {
    fileStream
        << "block,agegrp,sex,oud,counts" << std::endl
        << "No_Treatment,10_14,Male,Active_Noninjection,2917.55795376043"
        << std::endl
        << "No_Treatment,10_14,Male,Active_Injection,977.390032367151"
        << std::endl
        << "No_Treatment,10_14,Male,Nonactive_Noninjection,288.995723856067";
    fileStream.close();

    data_ops::DataLoader dl(std::filesystem::temp_directory_path().string(),
                            logger);

    dl.loadInitialSample(tempAbsoluteFile.string());

    data_ops::Matrix3d result = dl.getInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, setInitialSample) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 2917.55795376043;
    dl.setInitialSample(mat3d);

    data_ops::Matrix3d result = dl.getInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, setEnteringSamples) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 11.4389540364826;
    std::vector<data_ops::Matrix3d> mat4dVec{mat3d};
    data_ops::TimedMatrix3d mat4d(mat4dVec);
    dl.setEnteringSamples(mat4d);

    data_ops::TimedMatrix3d result = dl.getEnteringSamples();
    EXPECT_EQ(result(0, 0, 0, 0), 11.4389540364826);
}

TEST_F(DataLoaderTest, setOUDTransitionRates) {

    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.560720353446504;
    dl.setOUDTransitionRates(mat3d);

    data_ops::Matrix3d result = dl.getOUDTransitionRates();
    EXPECT_EQ(result(0, 0, 0), 0.560720353446504);
}

TEST_F(DataLoaderTest, setInterventionTransitionRates) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.625523912484771;
    std::vector<data_ops::Matrix3d> mat4dVec{mat3d};
    data_ops::TimedMatrix3d mat4d(mat4dVec);
    dl.setInterventionTransitionRates(mat4d);

    data_ops::TimedMatrix3d result = dl.getInterventionTransitionRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.625523912484771);
}

TEST_F(DataLoaderTest, setOverdoseRates) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.00059346577560159;
    std::vector<data_ops::Matrix3d> mat4dVec{mat3d};
    data_ops::TimedMatrix3d mat4d(mat4dVec);
    dl.setOverdoseRates(mat4d);

    data_ops::TimedMatrix3d result = dl.getOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.00059346577560159);
}

TEST_F(DataLoaderTest, setFatalOverdoseRates) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.216540329711774;
    std::vector<data_ops::Matrix3d> mat4dVec{mat3d};
    data_ops::TimedMatrix3d mat4d(mat4dVec);
    dl.setFatalOverdoseRates(mat4d);

    data_ops::TimedMatrix3d result = dl.getFatalOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.216540329711774);
}

TEST_F(DataLoaderTest, setMortalityRates) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 6.8407483245769285e-06;
    dl.setMortalityRates(mat3d);

    data_ops::Matrix3d result = dl.getMortalityRates();
    EXPECT_EQ(result(0, 0, 0), 6.8407483245769285e-06);
}

TEST_F(DataLoaderTest, setInterventionInitRates) {
    data_ops::DataLoader dl;

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 6.8407483245769285e-06;
    dl.setInterventionInitRates(mat3d);

    data_ops::Matrix3d result = dl.getInterventionInitRates();
    EXPECT_EQ(result(0, 0, 0), 6.8407483245769285e-06);
}
