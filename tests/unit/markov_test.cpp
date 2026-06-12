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

using ::testing::_;
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

TEST_F(MarkovTest, GetEmptyState) {
    auto result = markov->GetState();
    SUCCEED();
}

TEST_F(MarkovTest, GetAndSetState) {
    markov->SetState(state);
    auto result = markov->GetState();
    EXPECT_TRUE(result.isApprox(state));
}

TEST_F(MarkovTest, GetEmptyTransitionName) {
    auto names = markov->GetTransitionNames();
    ASSERT_EQ(names.size(), 0);
}

TEST_F(MarkovTest, TransitionNames) {
    // When Markov::AddTransition copies the transition it calls `clone()` on
    // the provided object. Make the mock return a heap-allocated mock that
    // will receive the `GetTransitionName()` call later.
    auto upmt = std::make_unique<NiceMock<MockTransition>>();
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

TEST_F(MarkovTest, RunTransitions) {
    // When Markov::AddTransition copies the transition it calls `clone()` on
    // the provided object. Make the mock return a heap-allocated mock that
    // will receive the `GetTransitionName()` call later.
    auto upmt = std::make_unique<NiceMock<MockTransition>>();
    auto clone = std::make_unique<NiceMock<MockTransition>>();
    EXPECT_CALL(*clone, Execute(_, _)).Times(1);
    EXPECT_CALL(*upmt, clone())
        .WillOnce(::testing::Return(::testing::ByMove(std::move(clone))));

    markov->AddTransition(std::move(upmt));
    markov->RunTransitions();
}

TEST_F(MarkovTest, RunTransitionsAccumulatesDefaultHistories) {
    markov->SetState(state);
    markov->RunTransitions();

    Eigen::VectorXd next_state = state * 2.0;
    markov->SetState(next_state);
    markov->RunTransitions();

    const auto histories = markov->GetHistories();
    ASSERT_EQ(histories.size(), 5u);

    const auto state_history = histories.at("state").GetStateAsVector();
    ASSERT_EQ(state_history.size(), 3u);
    EXPECT_TRUE(state_history[0].isApprox(state));
    EXPECT_TRUE(state_history[1].isApprox(state));
    EXPECT_TRUE(state_history[2].isApprox(next_state));

    const auto overdose_history =
        histories.at("total_overdose").GetStateAsVector();
    ASSERT_EQ(overdose_history.size(), 3u);
    EXPECT_TRUE(overdose_history[0].isZero());
    EXPECT_TRUE(overdose_history[1].isZero());
    EXPECT_TRUE(overdose_history[2].isZero());
}

TEST_F(MarkovTest, SparseHistoryCaptureRecordsRequestedAndFinalTimesteps) {
    markov->SetHistoryCaptureInterval(2);
    markov->SetFinalTimestep(5);
    markov->SetState(state);

    for (int step = 0; step < 5; ++step) {
        markov->RunTransitions();
    }

    const auto histories = markov->GetHistories();
    const auto &timesteps = histories.at("state").GetRecordedTimesteps();
    std::vector<int> expected = {0, 2, 4, 5};
    ASSERT_EQ(timesteps, expected);
}

TEST_F(MarkovTest, ClearHistoriesResetsTrackingState) {
    markov->SetHistoryCaptureInterval(2);
    markov->SetFinalTimestep(4);
    markov->SetState(state);

    markov->RunTransitions();
    markov->RunTransitions();
    markov->ClearHistories();

    Eigen::VectorXd next_state = state * 3.0;
    markov->SetState(next_state);
    markov->RunTransitions();

    const auto histories = markov->GetHistories();
    const auto &timesteps = histories.at("state").GetRecordedTimesteps();
    std::vector<int> expected = {0};
    ASSERT_EQ(timesteps, expected);

    const auto &states = histories.at("state").GetRecordedStates();
    ASSERT_EQ(states.size(), 1u);
    EXPECT_TRUE(states[0].isApprox(next_state));
}

TEST_F(MarkovTest, ClearTransitions) {
    // When Markov::AddTransition copies the transition it calls `clone()` on
    // the provided object. Make the mock return a heap-allocated mock that
    // will receive the `GetTransitionName()` call later.
    auto upmt = std::make_unique<NiceMock<MockTransition>>();
    auto clone = std::make_unique<NiceMock<MockTransition>>();
    ON_CALL(*clone, GetTransitionName())
        .WillByDefault(Return(std::string("test_transition")));
    EXPECT_CALL(*upmt, clone())
        .WillOnce(::testing::Return(::testing::ByMove(std::move(clone))));

    markov->AddTransition(std::move(upmt));
    markov->ClearTransitions();
    auto names = markov->GetTransitionNames();
    ASSERT_EQ(names.size(), 0);
}

TEST_F(MarkovTest, EmptyHistories) {
    auto result = markov->GetHistories();
    ASSERT_EQ(result.size(), 0);
}

TEST_F(MarkovTest, Histories) {
    std::map<std::string, History> hv;
    History h("temp", "test_logger");
    hv["temp"] = h;
    markov->SetHistories(hv);
    auto result = markov->GetHistories();
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result["temp"], h);
}

TEST_F(MarkovTest, ModelName) { ASSERT_EQ(markov->GetModelName(), "markov"); }

TEST_F(MarkovTest, LogName) { ASSERT_EQ(markov->GetLogName(), "test_logger"); }

} // namespace testing
} // namespace respond
