////////////////////////////////////////////////////////////////////////////////
// File: intervention_test.cpp                                                //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-05-06                                                  //
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

class InterventionTest : public ::testing::Test {
public:
    std::unique_ptr<Transition> tran;
    Eigen::VectorXd state;
    Eigen::MatrixXd tran_matrix;
    std::map<std::string, History> histories;

protected:
    void SetUp() override {
        tran =
            TransitionFactory::CreateTransition("intervention", "test_logger");
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;

        tran_matrix = Eigen::MatrixXd(3, 3);
        tran_matrix << 0.5f, 0.1f, 0.8f, 0.5f, 0.1f, 0.8f, 0.5f, 0.1f, 0.8f;
    }
    void TearDown() override { tran.reset(); }
};

TEST_F(InterventionTest, NoTransitionMatrices) {
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(InterventionTest, TooManyTransitionMatrices) {
    tran->AddTransitionMatrix(tran_matrix);
    tran->AddTransitionMatrix(tran_matrix);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(InterventionTest, NotSquareTransitionMatrix) {
    tran->AddTransitionMatrix(state);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(InterventionTest, GoodExecuteNoHistory) {
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto expected = tran_matrix * state;
    EXPECT_TRUE(result.isApprox(expected));
}

TEST_F(InterventionTest, GoodExecuteWriteHistory) {
    History h("intervention_admission", "test_logger");
    histories["intervention_admission"] = h;
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto expected_return = tran_matrix * state;
    auto expected_admissions =
        (expected_return - state).cwiseMax(Eigen::VectorXd::Zero(3));

    EXPECT_TRUE(result.isApprox(expected_return));
    EXPECT_TRUE(histories["intervention_admission"].HasPendingState());
    EXPECT_TRUE(histories["intervention_admission"].GetPendingState().isApprox(
        expected_admissions));
}
} // namespace testing
} // namespace respond