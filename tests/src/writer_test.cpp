////////////////////////////////////////////////////////////////////////////////
// File: writer_test.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/writer.hpp>

#include <fstream>
#include <memory>

#include <datamanagement/DataManagement.hpp>
#include <gtest/gtest.h>

#include "data_loader_mock.hpp"

using namespace respond::data_ops;
using ::testing::_;
using ::testing::Return;

class WriterTest : public ::testing::Test {
protected:
    std::unique_ptr<Writer> writer;
    void SetUp() override {
        std::ofstream config_file_stream("sim.conf");

        config_file_stream
            << "[simulation]" << std::endl
            << "duration = 52" << std::endl
            << "aging_interval = 260" << std::endl
            << "intervention_change_times = 52" << std::endl
            << "entering_sample_change_times = 52" << std::endl
            << "overdose_change_times = 52" << std::endl
            << "stratified_entering_cohort = false" << std::endl
            << std::endl
            << "[state]" << std::endl
            << "interventions = No_Treatment, Buprenorphine,"
               "Naltrexone, Methadone, Detox, Post-Buprenorphine,"
               "Post-Naltrexone, Post-Methadone, Post-Detox"
            << std::endl
            << "ouds = Active_Noninjection, Active_Injection,"
               "Nonactive_Noninjection, Nonactive_Injection"
            << std::endl
            << std::endl
            << "[demographic]" << std::endl
            << "age_groups = 1_100" << std::endl
            << "sex = other" << std::endl
            << std::endl
            << "[cost]" << std::endl
            << "cost_analysis = true" << std::endl
            << "cost_perspectives = healthcare" << std::endl
            << "discount_rate = 0.0025 " << std::endl
            << "reporting_interval = 1" << std::endl
            << "cost_utility_output_timesteps = 52 " << std::endl
            << "cost_category_outputs = false " << std::endl
            << std::endl
            << "[output] " << std::endl
            << "per_intervention_predictions = true " << std::endl
            << "general_outputs = false " << std::endl
            << "general_stats_output_timesteps = 52";
        config_file_stream.close();

        Data::IConfigablePtr config =
            std::make_shared<Data::Config>("sim.conf");
        writer = Writer::Create(config);
    }
    void TearDown() override { std::remove("sim.conf"); }
};

TEST_F(WriterTest, FactoryCreate) {
    auto w = Writer::Create(nullptr);
    EXPECT_NE(nullptr, w);
}

TEST_F(WriterTest, WriteInputData) {
    MockDataLoader data_loader;
    Matrix3d behavior(9, 16, 1);
    behavior.setConstant(0.5);

    Matrix3d intervention(81, 4, 1);
    intervention.setConstant(0.5);

    Matrix3d standard(9, 4, 1);
    standard.setConstant(0.1);

    // WriteOUDTransitionRates
    EXPECT_CALL(data_loader, GetOUDTransitionRates())
        .WillRepeatedly(Return(behavior));

    // WriteInterventionTransitionRates
    EXPECT_CALL(data_loader, GetInterventionTransitionRates(_))
        .WillRepeatedly(Return(intervention));

    // WriteInterventionInitRates
    EXPECT_CALL(data_loader, GetInterventionInitRates())
        .WillRepeatedly(Return(behavior));

    // WriteOverdoseRates
    EXPECT_CALL(data_loader, GetOverdoseRates(_))
        .WillRepeatedly(Return(standard));

    // WriteFatalOverdoseRates
    EXPECT_CALL(data_loader, GetFatalOverdoseRates(_))
        .WillRepeatedly(Return(standard));

    // AHHHHHH (I don't exactly have a better way to do this?)
    std::string expected =
        "intervention,agegrp,race,sex,initial_oud,Active_Noninjection1Active_"
        "Injection1Nonactive_Noninjection1Nonactive_Injection0 "
        "agegrp,race,sex,oud,initial_interventionNo_Treatment_1_"
        "52\nBuprenorphine_1_52\nNaltrexone_1_52\nMethadone_1_52\nDetox_1_"
        "52\nPost-Buprenorphine_1_52\nPost-Naltrexone_1_52\nPost-Methadone_1_"
        "52\nPost-Detox_1_52\n "
        "initial_oud_state,to_interventionActive_Noninjection1Active_"
        "Injection1Nonactive_Noninjection1Nonactive_Injection1Active_"
        "Noninjection,No_Treatment,0.500000,0.500000,0.500000,0.500000,"
        "\nActive_Noninjection,Buprenorphine,0.500000,0.500000,0.500000,0."
        "500000,\nActive_Noninjection,Naltrexone,0.500000,0.500000,0.500000,0."
        "500000,\nActive_Noninjection,Methadone,0.500000,0.500000,0.500000,0."
        "500000,\nActive_Noninjection,Detox,0.500000,0.500000,0.500000,0."
        "500000,\nActive_Noninjection,Post-Buprenorphine,0.500000,0.500000,0."
        "500000,0.500000,\nActive_Noninjection,Post-Naltrexone,0.500000,0."
        "500000,0.500000,0.500000,\nActive_Noninjection,Post-Methadone,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Noninjection,Post-Detox,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Injection,No_Treatment,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Injection,Buprenorphine,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Injection,Naltrexone,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Injection,Methadone,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Injection,Detox,0.500000,0."
        "500000,0.500000,0.500000,\nActive_Injection,Post-Buprenorphine,0."
        "500000,0.500000,0.500000,0.500000,\nActive_Injection,Post-Naltrexone,"
        "0.500000,0.500000,0.500000,0.500000,\nActive_Injection,Post-Methadone,"
        "0.500000,0.500000,0.500000,0.500000,\nActive_Injection,Post-Detox,0."
        "500000,0.500000,0.500000,0.500000,\nNonactive_Noninjection,No_"
        "Treatment,0.500000,0.500000,0.500000,0.500000,\nNonactive_"
        "Noninjection,Buprenorphine,0.500000,0.500000,0.500000,0.500000,"
        "\nNonactive_Noninjection,Naltrexone,0.500000,0.500000,0.500000,0."
        "500000,\nNonactive_Noninjection,Methadone,0.500000,0.500000,0.500000,"
        "0.500000,\nNonactive_Noninjection,Detox,0.500000,0.500000,0.500000,0."
        "500000,\nNonactive_Noninjection,Post-Buprenorphine,0.500000,0.500000,"
        "0.500000,0.500000,\nNonactive_Noninjection,Post-Naltrexone,0.500000,0."
        "500000,0.500000,0.500000,\nNonactive_Noninjection,Post-Methadone,0."
        "500000,0.500000,0.500000,0.500000,\nNonactive_Noninjection,Post-Detox,"
        "0.500000,0.500000,0.500000,0.500000,\nNonactive_Injection,No_"
        "Treatment,0.500000,0.500000,0.500000,0.500000,\nNonactive_Injection,"
        "Buprenorphine,0.500000,0.500000,0.500000,0.500000,\nNonactive_"
        "Injection,Naltrexone,0.500000,0.500000,0.500000,0.500000,\nNonactive_"
        "Injection,Methadone,0.500000,0.500000,0.500000,0.500000,\nNonactive_"
        "Injection,Detox,0.500000,0.500000,0.500000,0.500000,\nNonactive_"
        "Injection,Post-Buprenorphine,0.500000,0.500000,0.500000,0.500000,"
        "\nNonactive_Injection,Post-Naltrexone,0.500000,0.500000,0.500000,0."
        "500000,\nNonactive_Injection,Post-Methadone,0.500000,0.500000,0."
        "500000,0.500000,\nNonactive_Injection,Post-Detox,0.500000,0.500000,0."
        "500000,0.500000,\n intervention,agegrp,race,sex,oudoverdose_1_52, "
        "agegrp,race,sexpercent_overdoses_fatal_1_52,";

    std::string result =
        writer->WriteInputData(data_loader, "", OutputType::kString);
    ASSERT_EQ(expected, result);
}

TEST_F(WriterTest, WriteHistoryData) {}

TEST_F(WriterTest, WriteCostData) {}

TEST_F(WriterTest, WriteUtilityData) {}

TEST_F(WriterTest, WriteTotalsData) {}
