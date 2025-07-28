////////////////////////////////////////////////////////////////////////////////
// File: writer_test.cpp                                                      //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/postprocess/writer.hpp>

#include <fstream>
#include <memory>

#include <datamanagement/DataManagement.hpp>
#include <gtest/gtest.h>

#include <respond/preprocess/types.hpp>

#include "data_loader_mock.hpp"
#include "expected_strings.hpp"

using namespace respond::preprocess;
using ::testing::_;
using ::testing::Return;

class WriterTest : public ::testing::Test {
protected:
    std::unique_ptr<Writer> writer;
    Data::IConfigablePtr config;
    respond::tests::ExpectedStrings expected_strings;
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
            << "behaviors = Active_Noninjection, Active_Injection,"
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

        writer = Writer::Create();
    }
    void TearDown() override { std::remove("sim.conf"); }
};

TEST_F(WriterTest, WriteInputData) {
    MockDataLoader data_loader;
    Matrix3d behavior(9, 16, 1);
    behavior.setConstant(0.9);

    Matrix3d intervention(81, 4, 1);
    intervention.setConstant(0.8);

    Matrix3d standard(9, 4, 1);
    standard.setConstant(0.7);

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

    std::string expected = expected_strings.kWriterTestWriteInputDataExpected;

    std::string result =
        writer->WriteInputData(data_loader, "", OutputType::kString);
    ASSERT_EQ(expected, result);
}

TEST_F(WriterTest, WriteHistoryData) {
    History history;
    Matrix3d mat3d_1(9, 4, 1);
    Matrix3d mat3d_2(9, 4, 1);
    TimedMatrix3d timed_mat;

    // state_history
    mat3d_1.setConstant(0.9);
    mat3d_2.setConstant(0.8);
    timed_mat[0] = mat3d_1;
    timed_mat[52] = mat3d_2;
    history.state_history = timed_mat;

    // overdose_history
    mat3d_1.setConstant(0.7);
    mat3d_2.setConstant(0.6);
    timed_mat[0] = mat3d_1;
    timed_mat[1] = mat3d_2;
    history.overdose_history = timed_mat;

    // fatal_overdose_history
    mat3d_1.setConstant(0.5);
    mat3d_1.setConstant(0.4);
    timed_mat[0] = mat3d_1;
    timed_mat[1] = mat3d_2;
    history.fatal_overdose_history = timed_mat;

    // mortality_history
    mat3d_1.setConstant(0.3);
    mat3d_2.setConstant(0.2);
    timed_mat[0] = mat3d_1;
    timed_mat[1] = mat3d_2;
    history.mortality_history = timed_mat;

    // intervention_admission_history
    mat3d_1.setConstant(0.1);
    mat3d_2.setConstant(0.0);
    timed_mat[0] = mat3d_1;
    timed_mat[1] = mat3d_2;
    history.intervention_admission_history = timed_mat;

    std::string expected = expected_strings.kWriterTestWriteHistoryDataExpected;

    std::string result =
        writer->WriteHistoryData(history, "", OutputType::kString);
    ASSERT_EQ(expected, result);
}

TEST_F(WriterTest, WriteCostData) {
    CostList cost_list;
    Matrix3d mat3d_1(9, 4, 1);
    Matrix3d mat3d_2(9, 4, 1);
    TimedMatrix3d timed_mat;

    Cost cost;

    // healthcare_cost
    mat3d_1.setConstant(0.9);
    mat3d_2.setConstant(0.8);
    timed_mat[0] = mat3d_1;
    timed_mat[52] = mat3d_2;
    cost.healthcare_cost = timed_mat;

    // non_fatal_overdose_cost
    mat3d_1.setConstant(0.7);
    mat3d_2.setConstant(0.6);
    timed_mat[0] = mat3d_1;
    timed_mat[52] = mat3d_2;
    cost.non_fatal_overdose_cost = timed_mat;

    // fatal_overdose_cost
    mat3d_1.setConstant(0.5);
    mat3d_2.setConstant(0.4);
    timed_mat[0] = mat3d_1;
    timed_mat[52] = mat3d_2;
    cost.fatal_overdose_cost = timed_mat;

    // pharma_cost
    mat3d_1.setConstant(0.3);
    mat3d_2.setConstant(0.2);
    timed_mat[0] = mat3d_1;
    timed_mat[52] = mat3d_2;
    cost.pharma_cost = timed_mat;

    // treatment_cost
    mat3d_1.setConstant(0.1);
    mat3d_2.setConstant(0.0);
    timed_mat[0] = mat3d_1;
    timed_mat[52] = mat3d_2;
    cost.treatment_cost = timed_mat;

    cost.perspective = "healthcare";
    cost_list.push_back(cost);

    std::string expected = expected_strings.kWriterTestWriteCostDataExpected;

    std::string result =
        writer->WriteCostData(cost_list, "", OutputType::kString);

    ASSERT_EQ(expected, result);
}

TEST_F(WriterTest, WriteUtilityData) {
    TimedMatrix3d utilities;
    Matrix3d mat3d_1(9, 4, 1);
    Matrix3d mat3d_2(9, 4, 1);
    mat3d_1.setConstant(0.9);
    mat3d_2.setConstant(0.8);
    utilities[0] = mat3d_1;
    utilities[52] = mat3d_2;

    std::string expected = expected_strings.kWriterTestWriteUtilityDataExpected;

    std::string result =
        writer->WriteUtilityData(utilities, "", OutputType::kString);
    ASSERT_EQ(expected, result);
}

TEST_F(WriterTest, WriteTotalsData) {
    Totals totals;
    totals.base_costs = {100.0, 200.0, 300.0};
    totals.disc_costs = {90.0, 180.0, 270.0};
    totals.base_life_years = 100.0;
    totals.disc_life_years = 90.0;
    totals.base_utility = 0.8;
    totals.disc_utility = 0.7;

    std::string expected = expected_strings.kWriterTestWriteTotalsDataExpected;
    std::string result =
        writer->WriteTotalsData(totals, "", OutputType::kString);
    ASSERT_EQ(expected, result);
}

TEST_F(WriterTest, WriteTimedMatrix3dOutOfBoundsMatrixAccess) {
    TimedMatrix3d utilities;
    Matrix3d mat3d(1, 1, 1);
    mat3d.setConstant(0.9);
    utilities[0] = mat3d;

    ASSERT_THROW(writer->WriteUtilityData(utilities, "", OutputType::kString),
                 std::runtime_error);
}

TEST_F(WriterTest, WriteDirectoryDoesNotExist) {
    std::string fake_dir = "FakeDir";
    std::string expected = "failure\n\nfailure\n\nfailure\n\n"
                           "failure\n\nfailure";

    MockDataLoader data_loader;
    Matrix3d behavior(9, 16, 1);
    behavior.setConstant(0.9);

    Matrix3d intervention(81, 4, 1);
    intervention.setConstant(0.8);

    Matrix3d standard(9, 4, 1);
    standard.setConstant(0.7);

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

    std::string result =
        writer->WriteInputData(data_loader, fake_dir, OutputType::kFile);

    ASSERT_EQ(expected, result);
}
