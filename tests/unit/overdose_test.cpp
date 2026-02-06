////////////////////////////////////////////////////////////////////////////////
// File: overdose_test.cpp                                                    //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/transition.hpp>

#include <memory>

#include <Eigen/Dense>
#include <gtest/gtest.h>

#include <respond/history.hpp>
#include <respond/transition_factory.hpp>

namespace respond {
namespace testing {

class OverdoseTest : public ::testing::Test {
public:
    std::unique_ptr<Transition> tran;
    Eigen::VectorXd state;
    Eigen::VectorXd tran_matrix;
    std::map<std::string, History> histories;

protected:
    void SetUp() override {
        tran = TransitionFactory::CreateTransition("overdose", "test_logger");
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;

        tran_matrix = Eigen::VectorXd(3);
        tran_matrix << 0.5f, 0.1f, 0.8f;
    }
    void TearDown() override { tran.reset(); }
};

TEST_F(OverdoseTest, NoTransitionMatrices) {
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(OverdoseTest, TooFewTransitionMatrices) {
    tran->AddTransitionMatrix(state);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(OverdoseTest, TooManyTransitionMatrices) {
    tran->AddTransitionMatrix(state);
    tran->AddTransitionMatrix(state);
    tran->AddTransitionMatrix(state);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(OverdoseTest, WrongSizeTransitionMatrix) {
    Eigen::VectorXd bad_t_matrix;
    bad_t_matrix = Eigen::VectorXd(6);
    bad_t_matrix << 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f;
    tran->AddTransitionMatrix(bad_t_matrix);
    tran->AddTransitionMatrix(bad_t_matrix);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(OverdoseTest, GoodExecuteNoHistory) {
    tran->AddTransitionMatrix(tran_matrix);
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto overdoses = state.cwiseProduct(tran_matrix);
    auto fods = overdoses.cwiseProduct(tran_matrix);
    auto expected_return = state - fods;
    EXPECT_TRUE(result.isApprox(expected_return));
}

TEST_F(OverdoseTest, GoodExecuteWriteTotalOverdoseHistory) {
    History h("total_overdose", "test_logger");
    histories["total_overdose"] = h;
    tran->AddTransitionMatrix(tran_matrix);
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto od_result = histories["total_overdose"].GetStateAsVector()[0];

    auto overdoses = state.cwiseProduct(tran_matrix);
    auto fods = overdoses.cwiseProduct(tran_matrix);
    auto expected_return = state - fods;

    EXPECT_TRUE(result.isApprox(expected_return));
    EXPECT_TRUE(od_result.isApprox(overdoses));
}

TEST_F(OverdoseTest, GoodExecuteWriteFatalOverdoseHistory) {
    History h("fatal_overdose", "test_logger");
    histories["fatal_overdose"] = h;
    tran->AddTransitionMatrix(tran_matrix);
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto fod_result = histories["fatal_overdose"].GetStateAsVector()[0];

    auto overdoses = state.cwiseProduct(tran_matrix);
    auto fods = overdoses.cwiseProduct(tran_matrix);
    auto expected_return = state - fods;

    EXPECT_TRUE(fod_result.isApprox(fods));
    EXPECT_TRUE(result.isApprox(expected_return));
}

TEST_F(OverdoseTest, GoodExecuteWriteAllHistory) {
    History h1("fatal_overdose", "test_logger");
    histories["fatal_overdose"] = h1;
    History h2("total_overdose", "test_logger");
    histories["total_overdose"] = h2;
    tran->AddTransitionMatrix(tran_matrix);
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto od_result = histories["total_overdose"].GetStateAsVector()[0];
    auto fod_result = histories["fatal_overdose"].GetStateAsVector()[0];

    auto overdoses = state.cwiseProduct(tran_matrix);
    auto fods = overdoses.cwiseProduct(tran_matrix);
    auto expected_return = state - fods;

    EXPECT_TRUE(od_result.isApprox(overdoses));
    EXPECT_TRUE(fod_result.isApprox(fods));
    EXPECT_TRUE(result.isApprox(expected_return));
}
} // namespace testing
} // namespace respond
