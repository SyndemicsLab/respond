////////////////////////////////////////////////////////////////////////////////
// File: history_test.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-05-05                                                   //
// Author: GitHub Copilot                                                     //
// -----                                                                      //
// Last Modified: 2026-05-05                                                  //
// Modified By: GitHub Copilot                                                //
// -----                                                                      //
////////////////////////////////////////////////////////////////////////////////

#include <respond/history.hpp>

#include <vector>

#include <Eigen/Dense>
#include <gtest/gtest.h>

namespace respond {
namespace testing {

TEST(HistoryTest, SparseStoragePreservesRecordedTimesteps) {
    History history("state", "test_logger");
    Eigen::VectorXd state0(2);
    state0 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state0, 0);
    history.AddState(state2, 2);

    std::vector<int> expected_timesteps = {0, 2};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 2u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(state0));
    EXPECT_TRUE(history.GetRecordedStates()[1].isApprox(state2));
}

TEST(HistoryTest, GetStateAsVectorFillsSparseGapsWithZeros) {
    History history("state", "test_logger");
    Eigen::VectorXd state0(2);
    state0 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state0, 0);
    history.AddState(state2, 2);

    const auto dense_states = history.GetStateAsVector();
    ASSERT_EQ(dense_states.size(), 3u);
    EXPECT_TRUE(dense_states[0].isApprox(state0));
    EXPECT_TRUE(dense_states[1].isZero());
    EXPECT_TRUE(dense_states[2].isApprox(state2));
}

TEST(HistoryTest, GetStateMapBuildsSparseMapOnDemand) {
    History history("state", "test_logger");
    Eigen::VectorXd state0(1);
    state0 << 5.0f;
    Eigen::VectorXd state3(1);
    state3 << 7.0f;

    history.AddState(state0, 0);
    history.AddState(state3, 3);

    const auto state_map = history.GetStateMap();
    ASSERT_EQ(state_map.size(), 2u);
    EXPECT_TRUE(state_map.at(0).isApprox(state0));
    EXPECT_TRUE(state_map.at(3).isApprox(state3));
}

TEST(HistoryTest, ClearRemovesRecordedStatesAndTimesteps) {
    History history("state", "test_logger");
    Eigen::VectorXd state(1);
    state << 1.0f;
    history.AddState(state, 0);

    history.Clear();

    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
    EXPECT_TRUE(history.GetStateAsVector().empty());
    EXPECT_TRUE(history.GetStateMap().empty());
}

TEST(HistoryTest, AccumulatedHistoryFlushesPendingState) {
    History history("total_overdose", "test_logger", HistoryMode::Accumulated);
    Eigen::VectorXd first(2);
    first << 1.0f, 2.0f;
    Eigen::VectorXd second(2);
    second << 3.0f, 4.0f;

    history.AccumulateState(first);
    history.AccumulateState(second);
    history.FlushPendingState(4, 2);

    ASSERT_FALSE(history.HasPendingState());
    std::vector<int> expected_timesteps = {4};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);

    Eigen::VectorXd expected(2);
    expected << 4.0f, 6.0f;
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(expected));
}

TEST(HistoryTest, AccumulatedHistoryFlushesZeroWhenNoPendingStateExists) {
    History history("background_death", "test_logger",
                    HistoryMode::Accumulated);

    history.FlushPendingState(0, 3);

    std::vector<int> expected_timesteps = {0};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isZero());
}

} // namespace testing
} // namespace respond