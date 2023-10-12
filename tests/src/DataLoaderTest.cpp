//===-- DataLoaderTest.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:33 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <iostream>

#include "Configuration.hpp"
#include "DataLoader.hpp"

class DataLoaderTest : public ::testing::Test {
protected:
    boost::filesystem::path tempRelativeFile;
    boost::filesystem::path tempAbsoluteFile;
    boost::filesystem::path tempRelativeFile2;
    boost::filesystem::path tempAbsoluteFile2;
    boost::filesystem::path configFile;
    std::ofstream fileStream;
    std::ofstream fileStream2;
    std::ofstream configFileStream;
    void SetUp() override {
        tempRelativeFile =
            boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.csv");
        tempAbsoluteFile =
            boost::filesystem::temp_directory_path() / tempRelativeFile;

        tempRelativeFile2 =
            boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.csv");
        tempAbsoluteFile2 =
            boost::filesystem::temp_directory_path() / tempRelativeFile2;

        configFile = boost::filesystem::temp_directory_path() /
                     boost::filesystem::path("sim.conf");
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
                            "cost_perspectives = Healthcare System, Societal, Policymakers" 
                            << std::endl << 
                            "discount_rate = 0.0025 " 
                            << std::endl << 
                            "reporting_interval = 52 " 
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

TEST_F(DataLoaderTest, Constructor) {
    Data::DataLoader dl;
    EXPECT_EQ(dl.getInterventions().size(), 0);
}

TEST_F(DataLoaderTest, ConstructorInputDirectory) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, ConstructorIDandConfig) {
    Data::Configuration config(configFile.string());
    Data::DataLoader dl(config,
                        boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, loadConfigurationFile) {
    Data::DataLoader dl;
    EXPECT_EQ(dl.getInterventions().size(), 0);
    dl.loadConfigurationFile(configFile.string());
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, getDirName) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getDirName(),
              boost::filesystem::temp_directory_path().string());
}

TEST_F(DataLoaderTest, getDuration) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getDuration(), 52);
}

TEST_F(DataLoaderTest, getNumOUDStates) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getNumOUDStates(), 4);
}

TEST_F(DataLoaderTest, getNumInterventions) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getNumInterventions(), 9);
}

TEST_F(DataLoaderTest, getNumDemographics) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getNumDemographics(), 2);
}

TEST_F(DataLoaderTest, getNumDemographicCombos) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getNumDemographicCombos(), 36);
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

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadInitialSample(tempAbsoluteFile.string());

    Data::Matrix3d result = dl.getInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, enteringSamples) {
    fileStream << "agegrp,sex,number_of_new_comers_cycle52" << std::endl
               << "10_14,male,11.4389540364826" << std::endl
               << "10_14,female,7.10870959447953" << std::endl
               << "15_19,male,12.0934754686572";
    fileStream.close();

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadEnteringSamples(tempAbsoluteFile.string(),
                           std::string("No_Treatment"),
                           std::string("Active_Noninjection"));

    Data::Matrix3dOverTime result = dl.getEnteringSamples();
    EXPECT_EQ(result(0, 0, 0, 0), 11.4389540364826);
}

TEST_F(DataLoaderTest, OUDTransitionRates) {
    fileStream
        << "block,agegrp,sex,initial_status,to_Active_Noninjection,to_Active_"
           "Injection,to_Nonactive_Noninjection,to_Nonactive_Injection"
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

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadOUDTransitionRates(tempAbsoluteFile.string());

    Data::Matrix3d result = dl.getOUDTransitionRates();
    EXPECT_EQ(result(0, 0, 0), 0.560720353446504);
}

TEST_F(DataLoaderTest, interventionTransitionRates) {
    fileStream << "agegrp,sex,oud,initial_block,to_No_Treatment260,to_"
                  "Buprenorphine260,to_Naltrexone260,to_Methadone260,to_"
                  "Detox260,to_corresponding_post_trt260"
               << std::endl
               << "10_14,male,Active_Noninjection,No_Treatment,0."
                  "625523912484771,0.101388565684697,0.0472664681057711,0."
                  "178570136497494,0.0472509172272673,0"
               << std::endl
               << "10_14,male,Active_Noninjection,Buprenorphine,0,0."
                  "996990941661389,0,0,0,0.0030090583386112"
               << std::endl
               << "10_14,male,Active_Noninjection,Naltrexone,0,0,0."
                  "88186832069196,0,0,0.11813167930804";
    fileStream.close();

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadInterventionTransitionRates(tempAbsoluteFile.string());

    Data::Matrix3dOverTime result = dl.getInterventionTransitionRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.625523912484771);
}

TEST_F(DataLoaderTest, overdoseRates) {
    fileStream
        << "block,agegrp,sex,oud,all_types_overdose_cycle52,all_types_overdose_"
           "cycle104,all_types_overdose_cycle156,all_types_overdose_cycle208,"
           "all_types_overdose_cycle260"
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

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadOverdoseRates(tempAbsoluteFile.string());

    Data::Matrix3dOverTime result = dl.getOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.00059346577560159);
}

TEST_F(DataLoaderTest, fatalOverdoseRates) {
    fileStream << "fatal_to_all_types_overdose_ratio_cycle52,fatal_to_all_"
                  "types_overdose_ratio_cycle104,fatal_to_all_types_overdose_"
                  "ratio_cycle156,fatal_to_all_types_overdose_ratio_cycle208,"
                  "fatal_to_all_types_overdose_ratio_cycle260"
               << std::endl
               << "0.216540329711774,0.297741215749976,0.113841797135366,0."
                  "126092413319309,0.156049415151599";
    fileStream.close();

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadFatalOverdoseRates(tempAbsoluteFile.string());

    Data::Matrix3dOverTime result = dl.getFatalOverdoseRates();
    EXPECT_EQ(result(0, 0, 0, 0), 0.216540329711774);
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

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadMortalityRates(tempAbsoluteFile.string(),
                          tempAbsoluteFile2.string());

    Data::Matrix3d result = dl.getMortalityRates();
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

    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());

    dl.loadInitialSample(tempAbsoluteFile.string());

    Data::Matrix3d result = dl.getInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, getInterventions) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, getOUDStates) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getOUDStates().size(), 4);
}

TEST_F(DataLoaderTest, getAgingInterval) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getAgingInterval(), 260);
}

TEST_F(DataLoaderTest, getAgeGroupShift) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getAgeGroupShift(), 2);
}

TEST_F(DataLoaderTest, getCostSwitch) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_TRUE(dl.getCostSwitch());
}

TEST_F(DataLoaderTest, getCostPerspectives) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getCostPerspectives().size(), 3);
}

TEST_F(DataLoaderTest, getDiscountRate) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getDiscountRate(), 0.0025);
}

TEST_F(DataLoaderTest, getReportingInterval) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getReportingInterval(), 52);
}

TEST_F(DataLoaderTest, getCostCategoryOutputs) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_FALSE(dl.getCostCategoryOutputs());
}

TEST_F(DataLoaderTest, getPerInterventionPredictions) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_TRUE(dl.getPerInterventionPredictions());
}

TEST_F(DataLoaderTest, getGeneralOutputsSwitch) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_FALSE(dl.getGeneralOutputsSwitch());
}

TEST_F(DataLoaderTest, getGeneralStatsOutputTimesteps) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(dl.getGeneralStatsOutputTimesteps().size(), 1);
}
