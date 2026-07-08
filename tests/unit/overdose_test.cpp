////////////////////////////////////////////////////////////////////////////////
// File: overdose_test.cpp                                                    //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/history.hpp>
#include <respond/transition.hpp>

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "../../src/internals/overdose.hpp"

namespace respond {
namespace testing {

class OverdoseTest : public ::testing::Test {
public:
    Eigen::VectorXd state;
    Eigen::MatrixXd tran_matrix;
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

TEST_F(OverdoseTest, ExecuteNoMatrices) {
    Overdose overdose;
    histories["state"] = History("state");
    EXPECT_THROW((void)overdose.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Transition error - Wrong number of matrices. "
                             "Expected 2 transition matrix, got 0"));
}

TEST_F(OverdoseTest, ExecuteTooManyMatrices) {
    Overdose overdose;
    overdose.AddMatrix(tran_matrix);
    overdose.AddMatrix(tran_matrix);
    overdose.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    EXPECT_THROW((void)overdose.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "Transition error - Wrong number of matrices. "
                             "Expected 2 transition matrix, got 3"));
}

TEST_F(OverdoseTest, ExecuteSizeMismatch) {
    Overdose overdose;
    Eigen::MatrixXd wrong_size_matrix(2, 2);
    wrong_size_matrix << 0.5f, 0.1f, 0.8f, 0.6f;
    overdose.AddMatrix(wrong_size_matrix);
    overdose.AddMatrix(wrong_size_matrix);
    histories["state"] = History("state");
    EXPECT_THROW((void)overdose.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(
        FileContains(RESPOND_DEFAULT_LOG_FILE,
                     "Transition error - matrix size mismatch. "
                     "Matrix 1 size is (3, 1) but Matrix 2 size is (2, 2)"));
}

TEST_F(OverdoseTest, BadSecondMatrixSize) {
    Overdose overdose;
    overdose.AddMatrix(tran_matrix);
    Eigen::MatrixXd wrong_size_matrix(2, 2);
    wrong_size_matrix << 0.5f, 0.1f, 0.8f, 0.6f;
    overdose.AddMatrix(wrong_size_matrix);
    histories["state"] = History("state");
    EXPECT_THROW((void)overdose.Execute(state, histories), std::runtime_error);
    FlushAllLoggers();
    EXPECT_TRUE(
        FileContains(RESPOND_DEFAULT_LOG_FILE,
                     "Transition error - matrix size mismatch. "
                     "Matrix 1 size is (3, 1) but Matrix 2 size is (2, 2)"));
}

TEST_F(OverdoseTest, ExecuteValid) {
    Overdose overdose;
    overdose.AddMatrix(tran_matrix);
    overdose.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    Eigen::VectorXd result = overdose.Execute(state, histories);

    Eigen::VectorXd expected_overdoses = state.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_fods =
        expected_overdoses.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_new_state = state - expected_fods;
    EXPECT_TRUE(result.isApprox(expected_new_state));
}

TEST_F(OverdoseTest, ExecuteValidWithTotalOverdoseHistory) {
    Overdose overdose;
    overdose.AddMatrix(tran_matrix);
    overdose.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    histories["total_overdose"] = History("total_overdose");
    Eigen::VectorXd result = overdose.Execute(state, histories);

    Eigen::VectorXd expected_overdoses = state.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_fods =
        expected_overdoses.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_new_state = state - expected_fods;
    EXPECT_TRUE(result.isApprox(expected_new_state));

    histories["total_overdose"].FlushPendingState(0, state.size());

    // Check that the intervention_admission history has been updated correctly
    EXPECT_TRUE(histories["total_overdose"].GetStateAsVector()[0].isApprox(
        expected_overdoses));
}

TEST_F(OverdoseTest, ExecuteValidWithFatalOverdoseHistory) {
    Overdose overdose;
    overdose.AddMatrix(tran_matrix);
    overdose.AddMatrix(tran_matrix);
    histories["state"] = History("state");
    histories["fatal_overdose"] = History("fatal_overdose");
    Eigen::VectorXd result = overdose.Execute(state, histories);

    Eigen::VectorXd expected_overdoses = state.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_fods =
        expected_overdoses.cwiseProduct(tran_matrix);
    Eigen::VectorXd expected_new_state = state - expected_fods;
    EXPECT_TRUE(result.isApprox(expected_new_state));

    histories["fatal_overdose"].FlushPendingState(0, state.size());

    // Check that the intervention_admission history has been updated correctly
    EXPECT_TRUE(histories["fatal_overdose"].GetStateAsVector()[0].isApprox(
        expected_fods));
}

} // namespace testing
} // namespace respond
