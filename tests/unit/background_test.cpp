////////////////////////////////////////////////////////////////////////////////
// File: background_test.cpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/constants.hpp>
#include <respond/history.hpp>
#include <respond/logging.hpp>

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "../../src/internals/background.hpp"

namespace respond {
namespace testing {

class BackgroundDeathTest : public ::testing::Test {
public:
    Eigen::VectorXd state;
    Eigen::VectorXd tran_matrix;
    std::map<std::string, History> histories;

protected:
    void SetUp() override {
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;

        tran_matrix = Eigen::VectorXd(3);
        tran_matrix << 0.5f, 0.1f, 0.8f;

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

TEST_F(BackgroundDeathTest, DefaultConstructor) {
    BackgroundDeath background_death;
    EXPECT_EQ(background_death.GetName(), "background_death");
    ASSERT_EQ(CreateFileLogger(RESPOND_DEFAULT_LOG, default_log_file_),
              CreationStatus::kExists);
}

TEST_F(BackgroundDeathTest, ConstructorWithName) {
    BackgroundDeath background_death("custom_name");
    EXPECT_EQ(background_death.GetName(), "custom_name");
    ASSERT_EQ(CreateFileLogger(RESPOND_DEFAULT_LOG, default_log_file_),
              CreationStatus::kExists);
}

TEST_F(BackgroundDeathTest, ConstructorWithNameAndLogName) {
    BackgroundDeath background_death("custom_name", "custom_log");
    EXPECT_EQ(background_death.GetName(), "custom_name");
    ASSERT_EQ(CreateFileLogger("custom_log", default_log_file_),
              CreationStatus::kExists);
}

TEST_F(BackgroundDeathTest, ConstructorWithNameLogNameAndLogFile) {
    BackgroundDeath background_death("custom_name", "custom_log",
                                     test_log_file_);
    EXPECT_EQ(background_death.GetName(), "custom_name");
    ASSERT_EQ(CreateFileLogger("custom_log", test_log_file_),
              CreationStatus::kExists);
}

TEST_F(BackgroundDeathTest, ExecuteNoMatrices) {
    BackgroundDeath background_death;
    histories["state"] = History("state");
    EXPECT_THROW((void)background_death.Execute(state, histories),
                 std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Transition error - Wrong number of matrices. "
                             "Expected 1 transition matrix, got 0"));
}

TEST_F(BackgroundDeathTest, ExecuteTooManyMatrices) {
    BackgroundDeath background_death;
    background_death.AddMatrix(tran_matrix);
    background_death.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    EXPECT_THROW((void)background_death.Execute(state, histories),
                 std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Transition error - Wrong number of matrices. "
                             "Expected 1 transition matrix, got 2"));
}

TEST_F(BackgroundDeathTest, ExecuteNoBackgroundDeathHistory) {
    BackgroundDeath background_death;
    background_death.AddMatrix(tran_matrix);
    histories["state"] = History("state");

    auto deaths = state.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_state = state - deaths;
    Eigen::VectorXd result = background_death.Execute(state, histories);
    EXPECT_TRUE(result.isApprox(expected_state));
}

TEST_F(BackgroundDeathTest, ExecuteWithBackgroundDeathHistory) {
    BackgroundDeath background_death;
    background_death.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    histories["background_death"] = History("background_death");

    auto deaths = state.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_state = state - deaths;
    Eigen::VectorXd result = background_death.Execute(state, histories);
    EXPECT_TRUE(result.isApprox(expected_state));
}

TEST_F(BackgroundDeathTest, Clone) {
    BackgroundDeath background_death;
    background_death.AddMatrix(tran_matrix);
    std::unique_ptr<Transition> cloned_bgd = background_death.clone();
    EXPECT_EQ(cloned_bgd->GetName(), background_death.GetName());
    EXPECT_EQ(cloned_bgd->GetMatrices().size(),
              background_death.GetMatrices().size());
    EXPECT_TRUE(cloned_bgd->GetMatrices()[0].isApprox(
        background_death.GetMatrices()[0]));
}
} // namespace testing
} // namespace respond
