////////////////////////////////////////////////////////////////////////////////
// File: timestep_test.cpp                                                    //
// Project: respond                                                           //
// Created Date: 2026-07-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/timestep.hpp>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

namespace respond::testing {

class TimestepTest : public ::testing::Test {
public:
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
};

TEST_F(TimestepTest, DefaultConstructor) {
    Timestep ts;
    ASSERT_EQ(CreateFileLogger(RESPOND_DEFAULT_LOG, ""),
              CreationStatus::kExists);
}

TEST_F(TimestepTest, DefaultConstructorWithLogName) {
    std::string log_name = "temp";
    Timestep ts(log_name);
    ASSERT_EQ(CreateFileLogger(log_name, ""), CreationStatus::kExists);
}

TEST_F(TimestepTest, DefaultConstructorWithLogNameAndFile) {
    std::string log_name = "temp2";
    Timestep ts(log_name, test_log_file_);
    ASSERT_EQ(CreateFileLogger(log_name, test_log_file_),
              CreationStatus::kExists);
}

TEST_F(TimestepTest, CreateTransition) {
    Timestep ts("test_log", test_log_file_);
    const std::unique_ptr<Transition> &transition =
        ts.CreateTransition("migration");
    ASSERT_NE(transition, nullptr);
    ASSERT_EQ(transition->GetName(), "migration");
}

TEST_F(TimestepTest, AddMatrixToTransitionByIndex) {
    Timestep ts("test_log", test_log_file_);
    const std::unique_ptr<Transition> &transition =
        ts.CreateTransition("migration");
    Eigen::MatrixXd m(2, 2);
    m << 0.5, 0.5, 0.5, 0.5;
    ts.AddMatrixToTransition(0, m);
    ASSERT_EQ(transition->GetMatrices().size(), 1);
    ASSERT_TRUE(transition->GetMatrices()[0].isApprox(m));
}

TEST_F(TimestepTest, AddMatrixToTransitionByName) {
    Timestep ts("test_log", test_log_file_);
    const std::unique_ptr<Transition> &transition =
        ts.CreateTransition("migration");
    Eigen::MatrixXd m(2, 2);
    m << 0.5, 0.5, 0.5, 0.5;
    ts.AddMatrixToTransition("migration", m);
    ASSERT_EQ(transition->GetMatrices().size(), 1);
    ASSERT_TRUE(transition->GetMatrices()[0].isApprox(m));
}

TEST_F(TimestepTest, GetTransitionByIndex) {
    Timestep ts("test_log", test_log_file_);
    const std::unique_ptr<Transition> &transition =
        ts.CreateTransition("migration");
    const std::unique_ptr<Transition> &retrieved_transition =
        ts.GetTransition(0);
    ASSERT_EQ(retrieved_transition->GetName(), "migration");
}

TEST_F(TimestepTest, GetTransitionByName) {
    Timestep ts("test_log", test_log_file_);
    const std::unique_ptr<Transition> &transition =
        ts.CreateTransition("migration");
    const std::unique_ptr<Transition> &retrieved_transition =
        ts.GetTransition("migration");
    ASSERT_EQ(retrieved_transition->GetName(), "migration");
}

TEST_F(TimestepTest, GetTransitions) {
    Timestep ts("test_log", test_log_file_);
    ts.CreateTransition("migration");
    ts.CreateTransition("behavior");
    std::vector<std::unique_ptr<Transition>> transitions = ts.GetTransitions();
    ASSERT_EQ(transitions.size(), 2);
    ASSERT_EQ(transitions[0]->GetName(), "migration");
    ASSERT_EQ(transitions[1]->GetName(), "behavior");
}

TEST_F(TimestepTest, GetTransitionNames) {
    Timestep ts("test_log", test_log_file_);
    ts.CreateTransition("migration");
    ts.CreateTransition("behavior");
    std::vector<std::string> names = ts.GetTransitionNames();
    ASSERT_EQ(names.size(), 2);
    ASSERT_EQ(names[0], "migration");
    ASSERT_EQ(names[1], "behavior");
}

TEST_F(TimestepTest, CopyConstructor) {
    Timestep ts1("test_log", test_log_file_);
    ts1.CreateTransition("migration");
    Timestep ts2(ts1);
    std::vector<std::string> names = ts2.GetTransitionNames();
    ASSERT_EQ(names.size(), 1);
    ASSERT_EQ(names[0], "migration");
}

TEST_F(TimestepTest, CopyAssignment) {
    Timestep ts1("test_log", test_log_file_);
    ts1.CreateTransition("migration");
    Timestep ts2;
    ts2 = ts1;
    std::vector<std::string> names = ts2.GetTransitionNames();
    ASSERT_EQ(names.size(), 1);
    ASSERT_EQ(names[0], "migration");
}

TEST_F(TimestepTest, StreamOperatorOverload) {
    Timestep ts("test_log", test_log_file_);
    ts.CreateTransition("migration");
    std::stringstream ss;
    ss << ts;
    std::string output = ss.str();
    ASSERT_NE(
        output.find("Timestep with the following transitions:\n - migration\n"),
        std::string::npos);
}
} // namespace respond::testing