////////////////////////////////////////////////////////////////////////////////
// File: background_test.cpp                                                  //
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

class BackgroundDeathTest : public ::testing::Test {
public:
    std::unique_ptr<Transition> tran;
    Eigen::VectorXd state;
    Eigen::VectorXd tran_matrix;
    std::map<std::string, History> histories;

protected:
    void SetUp() override {
        tran = TransitionFactory::CreateTransition("background_death",
                                                   "test_logger");
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;

        tran_matrix = Eigen::VectorXd(3);
        tran_matrix << 0.5f, 0.1f, 0.8f;
    }
    void TearDown() override { tran.reset(); }
};

TEST_F(BackgroundDeathTest, NoTransitionMatrices) {
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(BackgroundDeathTest, TooManyTransitionMatrices) {
    tran->AddTransitionMatrix(state);
    tran->AddTransitionMatrix(state);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(BackgroundDeathTest, GoodExecuteNoHistory) {
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto expected = state - state.cwiseProduct(tran_matrix);
    EXPECT_TRUE(result.isApprox(expected));
}

TEST_F(BackgroundDeathTest, GoodExecuteWriteHistory) {
    History h("background_death", "test_logger");
    histories["background_death"] = h;
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto expected_deaths = state.cwiseProduct(tran_matrix);
    auto expected_return = state - expected_deaths;

    EXPECT_TRUE(result.isApprox(expected_return));
    EXPECT_TRUE(histories["background_death"].HasPendingState());
    EXPECT_TRUE(histories["background_death"].GetPendingState().isApprox(
        expected_deaths));
}
} // namespace testing
} // namespace respond
