////////////////////////////////////////////////////////////////////////////////
// File: markov_test.cpp                                                      //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <respond/model.hpp>

#include <memory>

#include <Eigen/Dense>
#include <gtest/gtest.h>

#include <respond/transition_factory.hpp>

#include "../mocks/transition_mock.hpp"

using ::testing::NiceMock;
using ::testing::Return;

namespace respond {
namespace testing {

class MarkovTest : public ::testing::Test {
public:
    std::unique_ptr<Model> markov;
    Eigen::VectorXd state;

protected:
    void SetUp() override {
        markov = Model::Create("markov", "test_logger");
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;
    }
    void TearDown() override { markov.reset(); }
};

TEST_F(MarkovTest, GetAndSetState) {
    markov->SetState(state);
    auto result = markov->GetState();
    EXPECT_TRUE(result.isApprox(state));
}

TEST_F(MarkovTest, TransitionNames) {
    auto upmt = std::make_unique<NiceMock<MockTransition>>();

    // When Markov::AddTransition copies the transition it calls `clone()` on
    // the provided object. Make the mock return a heap-allocated mock that
    // will receive the `GetTransitionName()` call later.
    auto clone = std::make_unique<NiceMock<MockTransition>>();
    EXPECT_CALL(*clone, GetTransitionName())
        .WillOnce(Return(std::string("test_transition")));
    EXPECT_CALL(*upmt, clone())
        .WillOnce(::testing::Return(::testing::ByMove(std::move(clone))));

    markov->AddTransition(std::move(upmt));
    auto names = markov->GetTransitionNames();
    ASSERT_EQ(names.size(), 1u);
    EXPECT_EQ(names[0], "test_transition");
}

// TEST_F(MarkovTest, ZeroDuration) {
//     markov->RunTransitions();
//     auto results = markov->GetRunResults();

//     auto expected = Eigen::VectorXd::Zero(0);
//     EXPECT_TRUE(results[0].state.isApprox(expected));
//     EXPECT_TRUE(results[0].intervention_admissions.isApprox(expected));
//     EXPECT_TRUE(results[0].total_overdoses.isApprox(expected));
// }

// TEST_F(MarkovTest, SingleStep) {
//     auto markov = Markov::Create("test_logger");
//     markov->SetState(Eigen::VectorXd::Ones(5));

//     Transition t;
//     t.SetCallback(test_func);
//     t.transition_matrices = {Eigen::VectorXd::Ones(5)};
//     markov->SetTransitions({t});

//     markov->Run(1);
//     auto results = markov->GetRunResults();

//     EXPECT_TRUE(results[1].state.isApprox(Eigen::VectorXd::Constant(5, 2.0)));
// }

// TEST_F(MarkovTest, MultipleTransitions) {
//     auto markov = Markov::Create("test_logger");
//     markov->SetState(Eigen::VectorXd::Ones(5));

//     Transition t;
//     t.SetCallback(test_func);
//     t.transition_matrices = {Eigen::VectorXd::Ones(5)};

//     markov->SetTransitions({t, t, t, t, t});
//     markov->Run(1);
//     auto results = markov->GetRunResults();

//     EXPECT_TRUE(results[1].state.isApprox(Eigen::VectorXd::Constant(5, 6.0)));
// }

// TEST_F(MarkovTest, CopyConstructor) {
//     auto markov = Markov::Create("test_logger");
//     auto markov_two = markov->clone();
//     EXPECT_EQ(markov_two->GetLoggerName(), "test_logger");
// }
} // namespace testing
} // namespace respond

// TODO: Need to test the respond transition functions!
