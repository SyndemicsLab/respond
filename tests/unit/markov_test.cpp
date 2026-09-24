////////////////////////////////////////////////////////////////////////////////
// File: markov_test.cpp                                                      //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-09-14                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <respond/model.hpp>

#include <fstream>
#include <memory>
#include <thread>

#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "../../src/internals/markov.hpp"
#include "../mocks/transition_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace respond {
namespace testing {

class MarkovTest : public ::testing::Test {
public:
    Eigen::VectorXd state;

protected:
    void SetUp() override {
        // Clear any existing loggers from previous tests
        spdlog::drop_all();

        // Create temporary log files for testing
        test_log_file_ = "/tmp/respond_test.log";
        shared_log_file_ = "/tmp/respond_shared.log";
        default_log_file_ = RESPOND_DEFAULT_LOG_FILE;

        // Remove test files if they exist
        std::remove(default_log_file_.c_str());
        std::remove(test_log_file_.c_str());
        std::remove(shared_log_file_.c_str());

        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;
    }
    void TearDown() override {
        // Clean up loggers
        spdlog::drop_all();

        // Remove test files
        std::remove(default_log_file_.c_str());
        std::remove(test_log_file_.c_str());
        std::remove(shared_log_file_.c_str());
    }

    std::string test_log_file_;
    std::string shared_log_file_;
    std::string default_log_file_;

    // Helper to check if file contains a string
    bool FileContains(const std::string &filepath, const std::string &search) {
        std::ifstream file(filepath);
        if (!file.is_open())
            return false;

        std::string line;
        while (std::getline(file, line)) {
            if (line.find(search) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

TEST_F(MarkovTest, CreateMarkovModel) {
    auto markov = Model::Create("markov");
    ASSERT_NE(markov, nullptr);
    ASSERT_EQ(CreateFileLogger(RESPOND_DEFAULT_LOG, default_log_file_),
              CreationStatus::kExists);
}

TEST_F(MarkovTest, CreateMarkovModelProcessorCount) {
    unsigned int processor_count =
        std::thread::hardware_concurrency() > 1
            ? (std::thread::hardware_concurrency() / 2)
            : 1;
    auto markov = Model::Create("markov", processor_count);
    ASSERT_NE(markov, nullptr);
    ASSERT_EQ(CreateFileLogger(RESPOND_DEFAULT_LOG, default_log_file_),
              CreationStatus::kExists);
}

TEST_F(MarkovTest, CreateMarkovModelExecutionConfig) {
    ExecutionConfig config;
    config.eigen_threads = 2;

    auto markov = Model::Create("markov", config);
    ASSERT_NE(markov, nullptr);
}

TEST_F(MarkovTest, MoveConstructor) {
    Markov markov("markov_source", RESPOND_DEFAULT_LOG);
    markov.SetState(state);
    markov.SetHistoryCaptureInterval(3);
    markov.SetFinalTimestep(7);
    markov.SetInitialHistoryRecorded(true);
    markov.CreateDefaultHistories();
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);

    const auto expected_name = markov.GetName();
    const auto expected_interval = markov.GetHistoryCaptureInterval();
    const auto expected_final_timestep = markov.GetFinalTimestep();
    const auto expected_initial_history = markov.GetInitialHistoryRecorded();
    const auto expected_history_count = markov.GetHistories().size();

    Markov moved_markov(std::move(markov));

    EXPECT_TRUE(moved_markov.GetState().isApprox(state));
    EXPECT_EQ(moved_markov.GetName(), expected_name);
    EXPECT_EQ(moved_markov.GetHistoryCaptureInterval(), expected_interval);
    EXPECT_EQ(moved_markov.GetFinalTimestep(), expected_final_timestep);
    EXPECT_EQ(moved_markov.GetInitialHistoryRecorded(),
              expected_initial_history);
    EXPECT_EQ(moved_markov.GetHistories().size(), expected_history_count);
    EXPECT_NO_THROW((void)moved_markov.GetTimestepAtIndex(0));

    EXPECT_TRUE(markov.GetHistories().empty());
    EXPECT_THROW((void)markov.GetTimestepAtIndex(0), std::out_of_range);
}

TEST_F(MarkovTest, MoveOperator) {
    Markov markov("markov_source", RESPOND_DEFAULT_LOG);
    markov.SetState(state);
    markov.SetHistoryCaptureInterval(3);
    markov.SetFinalTimestep(7);
    markov.SetInitialHistoryRecorded(true);
    markov.CreateDefaultHistories();
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);

    const auto expected_name = markov.GetName();
    const auto expected_interval = markov.GetHistoryCaptureInterval();
    const auto expected_final_timestep = markov.GetFinalTimestep();
    const auto expected_initial_history = markov.GetInitialHistoryRecorded();
    const auto expected_history_count = markov.GetHistories().size();

    Markov moved_markov = std::move(markov);

    EXPECT_TRUE(moved_markov.GetState().isApprox(state));
    EXPECT_EQ(moved_markov.GetName(), expected_name);
    EXPECT_EQ(moved_markov.GetHistoryCaptureInterval(), expected_interval);
    EXPECT_EQ(moved_markov.GetFinalTimestep(), expected_final_timestep);
    EXPECT_EQ(moved_markov.GetInitialHistoryRecorded(),
              expected_initial_history);
    EXPECT_EQ(moved_markov.GetHistories().size(), expected_history_count);
    EXPECT_NO_THROW((void)moved_markov.GetTimestepAtIndex(0));

    EXPECT_TRUE(markov.GetHistories().empty());
    EXPECT_THROW((void)markov.GetTimestepAtIndex(0), std::out_of_range);
}

TEST_F(MarkovTest, Clone) {
    Markov markov("markov_source", RESPOND_DEFAULT_LOG);
    markov.SetState(state);
    markov.SetHistoryCaptureInterval(3);
    markov.SetFinalTimestep(7);
    markov.SetInitialHistoryRecorded(true);
    markov.CreateDefaultHistories();
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);

    const auto expected_name = markov.GetName();
    const auto expected_interval = markov.GetHistoryCaptureInterval();
    const auto expected_final_timestep = markov.GetFinalTimestep();
    const auto expected_initial_history = markov.GetInitialHistoryRecorded();
    const auto expected_history_count = markov.GetHistories().size();

    auto cloned_markov = markov.clone();

    EXPECT_TRUE(cloned_markov->GetState().isApprox(state));
    EXPECT_EQ(cloned_markov->GetName(), expected_name);
    EXPECT_EQ(cloned_markov->GetHistoryCaptureInterval(), expected_interval);
    EXPECT_EQ(cloned_markov->GetFinalTimestep(), expected_final_timestep);
    EXPECT_EQ(cloned_markov->GetInitialHistoryRecorded(),
              expected_initial_history);
    EXPECT_EQ(cloned_markov->GetHistories().size(), expected_history_count);
    EXPECT_NO_THROW((void)cloned_markov->GetTimestepAtIndex(0));

    EXPECT_FALSE(markov.GetHistories().empty());
    EXPECT_NO_THROW((void)markov.GetTimestepAtIndex(0));
}

TEST_F(MarkovTest, GetTimestepAtIndexOutOfRange) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    EXPECT_THROW((void)markov.GetTimestepAtIndex(0), std::out_of_range);
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);
    EXPECT_NO_THROW((void)markov.GetTimestepAtIndex(0));
    EXPECT_THROW((void)markov.GetTimestepAtIndex(1), std::out_of_range);
}

TEST_F(MarkovTest, GetTimestepAtIndex) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    Timestep timestep1(RESPOND_DEFAULT_LOG);
    Timestep timestep2(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep1);
    markov.AddTimestep(timestep2);

    EXPECT_EQ(markov.GetTimestepAtIndex(0), timestep1);
    EXPECT_EQ(markov.GetTimestepAtIndex(1), timestep2);
}

TEST_F(MarkovTest, GetAndSetState) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.SetState(state);
    auto result = markov.GetState();
    EXPECT_TRUE(result.isApprox(state));
}

TEST_F(MarkovTest, GetName) {
    Markov markov("markov_test", RESPOND_DEFAULT_LOG);
    EXPECT_EQ(markov.GetName(), "markov_test");
}

TEST_F(MarkovTest, GetHistoriesCreateDefaultHistories) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.CreateDefaultHistories();
    const auto &histories = markov.GetHistories();
    EXPECT_EQ(histories.size(), 5u);
    EXPECT_TRUE(histories.find("state") != histories.end());
    EXPECT_TRUE(histories.find("total_overdose") != histories.end());
    EXPECT_TRUE(histories.find("fatal_overdose") != histories.end());
    EXPECT_TRUE(histories.find("intervention_admission") != histories.end());
    EXPECT_TRUE(histories.find("background_death") != histories.end());
}

TEST_F(MarkovTest, GetTimestep) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    EXPECT_EQ(markov.GetTimestep(), 0);
    Timestep timestep1(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep1);
    EXPECT_EQ(markov.GetTimestep(), 0);
    markov.RunTimestep();
    EXPECT_EQ(markov.GetTimestep(), 1);
}

TEST_F(MarkovTest, GetAndSetHistoryCaptureInterval) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    EXPECT_EQ(markov.GetHistoryCaptureInterval(), 1);
    markov.SetHistoryCaptureInterval(5);
    EXPECT_EQ(markov.GetHistoryCaptureInterval(), 5);
}

TEST_F(MarkovTest, GetAndSetFinalTimestep) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    EXPECT_EQ(markov.GetFinalTimestep(), -1);
    markov.SetFinalTimestep(10);
    EXPECT_EQ(markov.GetFinalTimestep(), 10);
}

TEST_F(MarkovTest, GetAndSetInitialHistoryRecorded) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    EXPECT_FALSE(markov.GetInitialHistoryRecorded());
    markov.SetInitialHistoryRecorded(true);
    EXPECT_TRUE(markov.GetInitialHistoryRecorded());
}

TEST_F(MarkovTest, AddTimestep) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);
    EXPECT_NO_THROW((void)markov.GetTimestepAtIndex(0));
}

TEST_F(MarkovTest, AddTimestepBeyondFinalTimestep) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.SetFinalTimestep(1);
    Timestep timestep1(RESPOND_DEFAULT_LOG);
    Timestep timestep2(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep1);
    markov.AddTimestep(timestep2);
    markov.SetInitialHistoryRecorded(true);
    markov.RunTimesteps();
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Only running timesteps up to duration value."));
}

TEST_F(MarkovTest, RunTimestep) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);
    EXPECT_EQ(markov.GetTimestep(), 0);
    markov.RunTimestep();
    EXPECT_EQ(markov.GetTimestep(), 1);
}

TEST_F(MarkovTest, RunTimestepEmptyTimestepVector) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.RunTimestep();
    FlushAllLoggers();
    EXPECT_TRUE(
        FileContains(RESPOND_DEFAULT_LOG_FILE,
                     "No timesteps available to run for model: markov"));
}

TEST_F(MarkovTest, RunTimestepIndex) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.SetState(state);

    Timestep timestep1(RESPOND_DEFAULT_LOG);
    Timestep timestep2(RESPOND_DEFAULT_LOG);
    Timestep timestep3(RESPOND_DEFAULT_LOG);

    Eigen::VectorXd timestep1_change(3);
    timestep1_change << 1.0, 0.0, 0.0;
    timestep1.CreateTransition("migration")->AddMatrix(timestep1_change);

    Eigen::VectorXd timestep2_change(3);
    timestep2_change << 10.0, 0.0, 0.0;
    timestep2.CreateTransition("migration")->AddMatrix(timestep2_change);

    Eigen::VectorXd timestep3_change(3);
    timestep3_change << 0.0, 20.0, 0.0;
    timestep3.CreateTransition("migration")->AddMatrix(timestep3_change);

    markov.AddTimestep(timestep1);
    markov.AddTimestep(timestep2);
    markov.AddTimestep(timestep3);
    EXPECT_EQ(markov.GetTimestep(), 0);

    markov.RunTimestep(1);
    EXPECT_EQ(markov.GetTimestep(), 2);
    Eigen::VectorXd expected_state = state;
    expected_state(0) += 10.0;
    EXPECT_TRUE(markov.GetState().isApprox(expected_state));

    markov.RunTimestep();
    EXPECT_EQ(markov.GetTimestep(), 3);
    expected_state(1) += 20.0;
    EXPECT_TRUE(markov.GetState().isApprox(expected_state));
}

TEST_F(MarkovTest, RunTimestepIndexOutOfRange) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);
    markov.RunTimestep(2);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(
        RESPOND_DEFAULT_LOG_FILE,
        "Current timestep exceeds available timesteps for model: markov"));
}

TEST_F(MarkovTest, RunTimesteps) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.SetInitialHistoryRecorded(true);
    Timestep timestep1(RESPOND_DEFAULT_LOG);
    Timestep timestep2(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep1);
    markov.AddTimestep(timestep2);
    EXPECT_EQ(markov.GetTimestep(), 0);
    markov.RunTimesteps();
    EXPECT_EQ(markov.GetTimestep(), 2);
}

TEST_F(MarkovTest, RunTimestepsWithFinalTimestep) {
    Markov markov;
    markov.SetInitialHistoryRecorded(true);
    markov.SetFinalTimestep(1);
    Timestep timestep1(RESPOND_DEFAULT_LOG);
    Timestep timestep2(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep1);
    markov.AddTimestep(timestep2);
    EXPECT_EQ(markov.GetTimestep(), 0);
    markov.RunTimesteps();
    EXPECT_EQ(markov.GetTimestep(), 1);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Only running timesteps up to duration value."));
}

TEST_F(MarkovTest, RunTimestepsRecordInitialHistory) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.SetInitialHistoryRecorded(false);
    Timestep timestep1(RESPOND_DEFAULT_LOG);
    Timestep timestep2(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep1);
    markov.AddTimestep(timestep2);
    EXPECT_EQ(markov.GetTimestep(), 0);
    markov.RunTimesteps();
    EXPECT_EQ(markov.GetTimestep(), 2);
}

TEST_F(MarkovTest, ClearTimesteps) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    Timestep timestep(RESPOND_DEFAULT_LOG);
    markov.AddTimestep(timestep);
    EXPECT_NO_THROW((void)markov.GetTimestepAtIndex(0));
    markov.ClearTimesteps();
    EXPECT_THROW((void)markov.GetTimestepAtIndex(0), std::out_of_range);
}

TEST_F(MarkovTest, ClearHistories) {
    Markov markov("markov", RESPOND_DEFAULT_LOG);
    markov.CreateDefaultHistories();
    EXPECT_FALSE(markov.GetHistories().empty());
    markov.ClearHistories();
    EXPECT_TRUE(markov.GetHistories().empty());
}

TEST_F(MarkovTest, MoveAssignmentReplacesDestinationState) {
    Markov source("source", RESPOND_DEFAULT_LOG);
    source.AddTimestep(Timestep(RESPOND_DEFAULT_LOG));
    source.AddTimestep(Timestep(RESPOND_DEFAULT_LOG));
    source.CreateDefaultHistories();

    Markov destination("destination", RESPOND_DEFAULT_LOG);
    destination.AddTimestep(Timestep(RESPOND_DEFAULT_LOG));
    destination.ClearHistories();
    destination = std::move(source);

    EXPECT_EQ(destination.GetName(), "source");
    EXPECT_NO_THROW((void)destination.GetTimestepAtIndex(1));
    EXPECT_THROW((void)destination.GetTimestepAtIndex(2), std::out_of_range);
    EXPECT_FALSE(destination.GetHistories().empty());
    EXPECT_TRUE(source.GetHistories().empty());
}
} // namespace testing
} // namespace respond
