////////////////////////////////////////////////////////////////////////////////
// File: migration_test.cpp                                                   //
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

class MigrationTest : public ::testing::Test {
public:
    std::unique_ptr<Transition> tran;
    Eigen::VectorXd state;
    Eigen::VectorXd tran_matrix;
    std::map<std::string, History> histories;

protected:
    void SetUp() override {
        tran = TransitionFactory::CreateTransition("migration", "test_logger");
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;

        tran_matrix = Eigen::VectorXd(3);
        tran_matrix << 1.0f, 2.0f, 3.0f;
    }
    void TearDown() override { tran.reset(); }
};

TEST_F(MigrationTest, NoTransitionMatrices) {
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(MigrationTest, TooManyTransitionMatrices) {
    tran->AddTransitionMatrix(state);
    tran->AddTransitionMatrix(state);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(MigrationTest, WrongSizeTransitionMatrix) {
    Eigen::VectorXd bad_t_matrix;
    bad_t_matrix = Eigen::VectorXd(6);
    bad_t_matrix << 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f;
    tran->AddTransitionMatrix(bad_t_matrix);
    EXPECT_THROW(tran->Execute(state, histories), std::runtime_error);
}

TEST_F(MigrationTest, GoodExecuteNoHistory) {
    tran->AddTransitionMatrix(tran_matrix);
    auto result = tran->Execute(state, histories);
    auto expected = state + tran_matrix;
    EXPECT_TRUE(result.isApprox(expected));
}
} // namespace testing
} // namespace respond