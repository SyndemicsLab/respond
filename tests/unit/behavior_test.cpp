////////////////////////////////////////////////////////////////////////////////
// File: behavior_test.cpp                                                    //
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
#include <respond/transition.hpp>

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "../../src/internals/behavior.hpp"

namespace respond {
namespace testing {

class BehaviorTest : public ::testing::Test {
public:
    Eigen::VectorXd state;
    Eigen::MatrixXd tran_matrix;
    std::map<std::string, History> histories;

protected:
    void SetUp() override {
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;

        tran_matrix = Eigen::MatrixXd(1, 3);
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

TEST_F(BehaviorTest, ExecuteNoMatrices) {
    Behavior behavior;
    histories["state"] = History("state");
    EXPECT_THROW((void)behavior.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Transition error - Wrong number of matrices. "
                             "Expected 1 transition matrix, got 0"));
}

TEST_F(BehaviorTest, ExecuteTooManyMatrices) {
    Behavior behavior;
    behavior.AddMatrix(tran_matrix);
    behavior.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    EXPECT_THROW((void)behavior.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Transition error - Wrong number of matrices. "
                             "Expected 1 transition matrix, got 2"));
}

TEST_F(BehaviorTest, ExecuteDimensionMismatch) {
    Behavior behavior;
    Eigen::MatrixXd wrong_dim_matrix(3, 2); // 3 rows, 2 cols
    wrong_dim_matrix << 0.5f, 0.1f, 0.8f, 0.2f, 0.3f, 0.4f;
    behavior.AddMatrix(wrong_dim_matrix);
    histories["state"] = History("state");
    EXPECT_THROW((void)behavior.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(
        FileContains(RESPOND_DEFAULT_LOG_FILE,
                     "Transition error - Dimension mismatch, m1 rows do not "
                     "match m2 columns. m1 size is (3, 1) m2 size is (3, 2)"));
}

TEST_F(BehaviorTest, ExecuteValid) {
    Behavior behavior;
    behavior.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    Eigen::VectorXd new_state = behavior.Execute(state, histories);
    Eigen::VectorXd expected_state = tran_matrix * state;
    EXPECT_TRUE(new_state.isApprox(expected_state));
}

TEST_F(BehaviorTest, Clone) {
    Behavior behavior;
    behavior.AddMatrix(tran_matrix);
    std::unique_ptr<Transition> cloned_behavior = behavior.clone();
    EXPECT_EQ(cloned_behavior->GetName(), behavior.GetName());
    EXPECT_EQ(cloned_behavior->GetMatrices().size(),
              behavior.GetMatrices().size());
    EXPECT_TRUE(
        cloned_behavior->GetMatrices()[0].isApprox(behavior.GetMatrices()[0]));
}

} // namespace testing
} // namespace respond