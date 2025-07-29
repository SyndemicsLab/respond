////////////////////////////////////////////////////////////////////////////////
// File: data_loader_test.cpp                                                 //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/preprocess/data_loader.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <config.hpp>

using namespace respond::preprocess;

namespace respond {
namespace testing {

class DataLoaderTest : public ::testing::Test {
protected:
    const std::string file_name_1 = "data_test1.csv";
    const std::string file_name_2 = "data_test2.csv";
    void SetUp() override { BuildSimConf("sim.conf"); }
    void TearDown() override {
        std::remove(file_name_1.c_str());
        std::remove(file_name_2.c_str());
        std::remove("sim.conf");
    }
};

TEST_F(DataLoaderTest, initialSample) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream
        << "block,agegrp,sex,oud,counts" << std::endl
        << "No_Treatment,10_14,Male,Active_Noninjection,2917.55795376043"
        << std::endl
        << "No_Treatment,10_14,Male,Active_Injection,977.390032367151"
        << std::endl
        << "No_Treatment,10_14,Male,Nonactive_Noninjection,288.995723856067";
    file_stream.close();

    data_loader->LoadInitialSample(file_name_1);

    Matrix3d result = data_loader->GetInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, enteringSamples) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream << "agegrp,sex,cohort_size_change_1_52" << std::endl
                << "10_14,male,11.4389540364826" << std::endl
                << "10_14,female,7.10870959447953" << std::endl
                << "15_19,male,12.0934754686572";
    file_stream.close();

    data_loader->LoadEnteringSamples(file_name_1, std::string("No_Treatment"),
                                     std::string("Active_Noninjection"));

    auto result = data_loader->GetEnteringSamples(0);
    EXPECT_EQ(result(0, 0, 0), 11.4389540364826);
}

TEST_F(DataLoaderTest, OUDTransitionRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream
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
    file_stream.close();

    data_loader->LoadOUDTransitionRates(file_name_1);

    Matrix3d result = data_loader->GetOUDTransitionRates();
    EXPECT_EQ(result(0, 0, 0), 0.560720353446504);
}

TEST_F(DataLoaderTest, interventionTransitionRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream << "agegrp,sex,oud,initial_intervention,No_Treatment_1_52,"
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
    file_stream.close();

    data_loader->LoadInterventionTransitionRates(file_name_1);

    auto result = data_loader->GetInterventionTransitionRates(0);
    EXPECT_EQ(result(0, 0, 0), 0.625523912484771);
}

TEST_F(DataLoaderTest, overdoseRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream
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
    file_stream.close();

    data_loader->LoadOverdoseRates(file_name_1);

    auto result = data_loader->GetOverdoseRates(0);
    EXPECT_EQ(result(0, 0, 0), 0.00059346577560159);
}

TEST_F(DataLoaderTest, fatalOverdoseRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream
        << "percent_overdoses_fatal_1_52,percent_overdoses_fatal_52_104,"
           "percent_overdoses_fatal_104_156,percent_overdoses_fatal_156_208,"
           "percent_overdoses_fatal_208_260"
        << std::endl
        << "0.216540329711774,0.297741215749976,0.113841797135366,0."
           "126092413319309,0.156049415151599";
    file_stream.close();

    data_loader->LoadFatalOverdoseRates(file_name_1);

    auto result = data_loader->GetFatalOverdoseRates(0);
    EXPECT_EQ(result(0, 0, 0), 0.216540329711774);
}

TEST_F(DataLoaderTest, mortalityRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream_1(file_name_1);
    file_stream_1
        << "block,agegrp,sex,oud,SMR" << std::endl
        << "No_Treatment,10_14,male,Active_Noninjection,2.1203387149855"
        << std::endl
        << "No_Treatment,10_14,male,Active_Injection,5.51986056060266"
        << std::endl
        << "No_Treatment,10_14,male,Nonactive_Noninjection,1.83401409064184";
    file_stream_1.close();

    std::ofstream file_stream_2(file_name_2);
    file_stream_2 << "agegrp,sex,death_prob" << std::endl
                  << "10_14,Male,3.22625844595414E-06" << std::endl
                  << "10_14,Female,2.3250579153089E-06" << std::endl
                  << "15_19,Male,1.2191975906517E-05";
    file_stream_2.close();

    data_loader->LoadMortalityRates(file_name_1, file_name_2);

    Matrix3d result = data_loader->GetMortalityRates();
    EXPECT_EQ(result(0, 0, 0), 6.8407483245769285e-06);
}

TEST_F(DataLoaderTest, interventionInitRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);

    file_stream << "agegrp,sex,initial_oud_state,to_intervention,Active_"
                   "Noninjection,Active_Injection,Nonactive_Noninjection,"
                   "Nonactive_Injection"
                << std::endl
                << "10_14,Male,Active_Noninjection,No_Treatment,"
                   "0.4,0.3,0.2,0.1";
    file_stream.close();

    data_loader->LoadInterventionInitRates(file_name_1);

    Matrix3d result = data_loader->GetInterventionInitRates();
    EXPECT_EQ(result(0, 0, 0), 0.4);
}

TEST_F(DataLoaderTest, setInitialSample) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 2917.55795376043;
    data_loader->SetInitialSample(mat3d);

    Matrix3d result = data_loader->GetInitialSample();
    EXPECT_EQ(result(0, 0, 0), 2917.55795376043);
}

TEST_F(DataLoaderTest, setEnteringSamples) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 11.4389540364826;
    TimedMatrix3d mat4d = {{0, mat3d}};
    data_loader->SetEnteringSamples(mat4d);

    auto result = data_loader->GetEnteringSamples(0);
    EXPECT_EQ(result(0, 0, 0), 11.4389540364826);
}

TEST_F(DataLoaderTest, setOUDTransitionRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.560720353446504;
    data_loader->SetOUDTransitionRates(mat3d);

    Matrix3d result = data_loader->GetOUDTransitionRates();
    EXPECT_EQ(result(0, 0, 0), 0.560720353446504);
}

TEST_F(DataLoaderTest, setInterventionTransitionRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.625523912484771;
    TimedMatrix3d mat4d = {{0, mat3d}};
    data_loader->SetInterventionTransitionRates(mat4d);

    auto result = data_loader->GetInterventionTransitionRates(0);
    EXPECT_EQ(result(0, 0, 0), 0.625523912484771);
}

TEST_F(DataLoaderTest, setOverdoseRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.00059346577560159;
    TimedMatrix3d mat4d = {{0, mat3d}};
    data_loader->SetOverdoseRates(mat4d);

    auto result = data_loader->GetOverdoseRates(0);
    EXPECT_EQ(result(0, 0, 0), 0.00059346577560159);
}

TEST_F(DataLoaderTest, setFatalOverdoseRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.216540329711774;
    TimedMatrix3d mat4d = {{0, mat3d}};
    data_loader->SetFatalOverdoseRates(mat4d);

    auto result = data_loader->GetFatalOverdoseRates(0);
    EXPECT_EQ(result(0, 0, 0), 0.216540329711774);
}

TEST_F(DataLoaderTest, setMortalityRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 6.8407483245769285e-06;
    data_loader->SetMortalityRates(mat3d);

    Matrix3d result = data_loader->GetMortalityRates();
    EXPECT_EQ(result(0, 0, 0), 6.8407483245769285e-06);
}

TEST_F(DataLoaderTest, setInterventionInitRates) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 6.8407483245769285e-06;
    data_loader->SetInterventionInitRates(mat3d);

    Matrix3d result = data_loader->GetInterventionInitRates();
    EXPECT_EQ(result(0, 0, 0), 6.8407483245769285e-06);
}

TEST_F(DataLoaderTest, getConfig) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    auto config = data_loader->GetConfig();

    Data::ReturnType tmp_int = 1;
    Data::ReturnType expected = 52;
    EXPECT_EQ(config->get("simulation.duration", tmp_int), expected);
}

TEST_F(DataLoaderTest, getConfigNoSetConfig) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    auto config = data_loader->GetConfig();

    EXPECT_EQ(config, nullptr);
}

TEST_F(DataLoaderTest, enteringSamplesFileOnly) {
    std::unique_ptr<DataLoader> data_loader = DataLoader::Create();
    data_loader->SetConfig("sim.conf");
    std::ofstream file_stream(file_name_1);
    file_stream << "block,agegrp,sex,oud,cohort_size_change_1_52" << std::endl
                << "No_Treatment,10_14,male,Active_Noninjection,"
                   "11.4389540364826"
                << std::endl
                << "No_Treatment,10_14,female,Active_Noninjection,"
                   "7.10870959447953"
                << std::endl
                << "No_Treatment,15_19,male,Active_Noninjection,"
                   "12.0934754686572";
    file_stream.close();

    data_loader->LoadEnteringSamples(file_name_1);

    auto result = data_loader->GetEnteringSamples(0);
    EXPECT_EQ(result(0, 0, 0), 11.4389540364826);
}
} // namespace testing
} // namespace respond
