////////////////////////////////////////////////////////////////////////////////
// File: respond_test.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-16                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/respond.hpp>

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "markov_mock.hpp"

using namespace respond;

class RespondTest : public ::testing::Test {
protected:
    Eigen::VectorXd state;
    std::vector<Eigen::MatrixXd> transition_matrices;
    void SetUp() override {
        state = Eigen::VectorXd::Ones(6);
        transition_matrices.clear();
    }
    void TearDown() override {}
};

TEST_F(RespondTest, Migration_Function) {

    Eigen::VectorXd t(6);
    t << -1, 0, 1, 0, 0, 0;
    transition_matrices.push_back(t);

    auto result = Migration(state, transition_matrices);
    Eigen::VectorXd expected(6);
    expected << 0, 1, 2, 1, 1, 1;

    EXPECT_TRUE(result.isApprox(expected));
}

TEST_F(RespondTest, Behavior_Function) {

    Eigen::MatrixXd t(6, 6);
    // clang-format off
    t << 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.9, 0.2, 0.0, 0.0,
         0.0, 0.0, 0.1, 0.8, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
    // clang-format on
    transition_matrices.push_back(t);

    auto result = Behavior(state, transition_matrices);
    Eigen::VectorXd expected(6);
    expected << 1.0, 1.0, 1.1, 0.9, 1.0, 1.0;

    EXPECT_TRUE(result.isApprox(expected));
}

TEST_F(RespondTest, Intervention_Function) {

    Eigen::MatrixXd t1(6, 6);
    // clang-format off
    t1 << 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.2, 0.1, 0.0, 0.0,
          0.0, 0.0, 0.8, 0.9, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
    // clang-format on

    transition_matrices.push_back(t1);

    auto result = Intervention(state, transition_matrices);
    Eigen::VectorXd expected(6);
    expected << 1.0, 1.0, 0.3, 1.7, 1.0, 1.0;
    EXPECT_TRUE(result.isApprox(expected));
}

TEST_F(RespondTest, Overdose_Function) {

    Eigen::VectorXd t1(6);
    t1 << 0.0, 0.0, 0.4, 0.2, 0.0, 0.0;
    Eigen::VectorXd t2 = Eigen::VectorXd::Constant(6, 0.25);

    transition_matrices.push_back(t1);
    transition_matrices.push_back(t2);

    auto result = Overdose(state, transition_matrices);
    Eigen::VectorXd expected(6);
    expected << 1.0, 1.0, 0.9, 0.95, 1.1, 1.05;

    EXPECT_TRUE(state.isApprox(expected));
    EXPECT_TRUE(result.isApprox(t1));
}

TEST_F(RespondTest, Mortality_Function) {
    Eigen::MatrixXd t(6, 6);
    // clang-format off
    t << 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.9, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.2, 0.0, 0.0,
         0.0, 0.0, 0.1, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 0.8, 0.0, 1.0;
    // clang-format on
    transition_matrices.push_back(t);

    auto result = Mortality(state, transition_matrices);
    Eigen::VectorXd expected(6);
    expected << 1.0, 1.0, 0.9, 0.2, 1.1, 1.8;

    EXPECT_TRUE(result.isApprox(expected));
}
