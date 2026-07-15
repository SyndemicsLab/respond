////////////////////////////////////////////////////////////////////////////////
// File: history_test.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-05-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
////////////////////////////////////////////////////////////////////////////////

#include <respond/history.hpp>

#include <fstream>
#include <vector>

#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

namespace respond {
namespace testing {

class HistoryTest : public ::testing::Test {
public:
protected:
    void SetUp() override {
        // Clear any existing loggers from previous tests
        spdlog::drop_all();

        // Create temporary log files for testing
        default_log_file_ = RESPOND_DEFAULT_LOG_FILE;

        // Remove test files if they exist
        std::remove(default_log_file_.c_str());
    }
    void TearDown() override {
        // Clean up loggers
        spdlog::drop_all();

        // Remove test files
        std::remove(default_log_file_.c_str());
    }

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

TEST_F(HistoryTest, GetDefaultHistoryModeReturnsAccumulatedForSpecificNames) {
    EXPECT_EQ(GetDefaultHistoryMode("intervention_admission"),
              HistoryMode::kAccumulated);
    EXPECT_EQ(GetDefaultHistoryMode("total_overdose"),
              HistoryMode::kAccumulated);
    EXPECT_EQ(GetDefaultHistoryMode("fatal_overdose"),
              HistoryMode::kAccumulated);
    EXPECT_EQ(GetDefaultHistoryMode("background_death"),
              HistoryMode::kAccumulated);
}

TEST_F(HistoryTest, GetDefaultHistoryModeReturnsSnapshotForOtherNames) {
    EXPECT_EQ(GetDefaultHistoryMode("state"), HistoryMode::kSnapshot);
    EXPECT_EQ(GetDefaultHistoryMode("custom_history"), HistoryMode::kSnapshot);
    EXPECT_EQ(GetDefaultHistoryMode(""), HistoryMode::kSnapshot);
}

TEST_F(HistoryTest, DefaultConstructor) {
    History history;
    EXPECT_EQ(history.GetName(), "state");
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kSnapshot);
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, ConstructorWithName) {
    History history("custom_history");
    EXPECT_EQ(history.GetName(), "custom_history");
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kSnapshot);
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, ConstructorWithNameAndMode) {
    History history("total_overdose", HistoryMode::kAccumulated);
    EXPECT_EQ(history.GetName(), "total_overdose");
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kAccumulated);
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, ConstructorWithNameModeAndLogger) {
    History history("fatal_overdose", HistoryMode::kAccumulated, "test_logger");
    EXPECT_EQ(history.GetName(), "fatal_overdose");
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kAccumulated);
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, ConstructorWithNameAndLogger) {
    History history("background_death", "test_logger");
    EXPECT_EQ(history.GetName(), "background_death");
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kAccumulated);
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, ConstructorWithNameModeLoggerAndLogFile) {
    History history("background_death", HistoryMode::kAccumulated,
                    "test_logger", default_log_file_);
    EXPECT_EQ(history.GetName(), "background_death");
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kAccumulated);
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, EqualityOperator) {
    History history("state");
    History history_copy("state");

    history.AddState(Eigen::VectorXd::Ones(2), 0);
    history_copy.AddState(Eigen::VectorXd::Ones(2), 0);
    EXPECT_EQ(history, history_copy);
}

TEST_F(HistoryTest, InequalityOperator) {
    History history("state");
    History history_copy("state");

    history.AddState(Eigen::VectorXd::Ones(2), 0);
    history_copy.AddState(Eigen::VectorXd::Zero(2), 0);
    EXPECT_NE(history, history_copy);
}

TEST_F(HistoryTest, AddStateRecordsStateAtSpecifiedTimestep) {
    History history("state");
    Eigen::VectorXd state(2);
    state << 1.0f, 2.0f;

    history.AddState(state, 5);

    std::vector<int> expected_timesteps = {5};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(state));
}

TEST_F(HistoryTest, AddStateRecordsStateAtNextSequentialTimestep) {
    History history("state");
    Eigen::VectorXd state1(2);
    state1 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state1); // Should be at timestep 0
    history.AddState(state2); // Should be at timestep 1

    std::vector<int> expected_timesteps = {0, 1};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 2u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(state1));
    EXPECT_TRUE(history.GetRecordedStates()[1].isApprox(state2));
}

TEST_F(HistoryTest, AccumulateStateAddsToPendingState) {
    History history("total_overdose", HistoryMode::kAccumulated);
    Eigen::VectorXd first(2);
    first << 1.0f, 2.0f;
    Eigen::VectorXd second(2);
    second << 3.0f, 4.0f;

    history.AccumulateState(first);
    history.AccumulateState(second);

    Eigen::VectorXd expected(2);
    expected << 4.0f, 6.0f;
    EXPECT_TRUE(history.GetPendingState().isApprox(expected));
}

TEST_F(HistoryTest, AccumulateStateOnNonAccumulatedHistoryAddsState) {
    History history("state", HistoryMode::kSnapshot);
    Eigen::VectorXd state(2);
    state << 1.0f, 2.0f;

    history.AccumulateState(state);

    std::vector<int> expected_timesteps = {0};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(state));
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "AccumulateState called on non-accumulated"));
}

TEST_F(HistoryTest, FlushPendingStateRecordsPendingStateAtSpecifiedTimestep) {
    History history("total_overdose", HistoryMode::kAccumulated);
    Eigen::VectorXd first(2);
    first << 1.0f, 2.0f;
    Eigen::VectorXd second(2);
    second << 3.0f, 4.0f;

    history.AccumulateState(first);
    history.AccumulateState(second);
    history.FlushPendingState(10, 2);

    ASSERT_FALSE(history.HasPendingState());
    std::vector<int> expected_timesteps = {10};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);

    Eigen::VectorXd expected(2);
    expected << 4.0f, 6.0f;
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(expected));
}

TEST_F(HistoryTest, FlushPendingStateOnNonAccumulatedHistoryDoesNothing) {
    History history("state", HistoryMode::kSnapshot);
    Eigen::VectorXd state(2);
    state << 1.0f, 2.0f;

    history.AccumulateState(state);  // Should add state instead
    history.FlushPendingState(5, 2); // Should do nothing

    std::vector<int> expected_timesteps = {0};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(state));
    FlushAllLoggers();
    EXPECT_TRUE(
        FileContains(RESPOND_DEFAULT_LOG_FILE,
                     "FlushPendingState called on non-accumulated history"));
}

TEST_F(HistoryTest, FlushPendingStateWithNoPendingStateRecordsZeroVector) {
    History history("total_overdose", HistoryMode::kAccumulated);
    history.FlushPendingState(3, 2);

    std::vector<int> expected_timesteps = {3};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 1u);

    Eigen::VectorXd expected = Eigen::VectorXd::Zero(2);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(expected));
}

TEST_F(HistoryTest, ClearEmptiesHistory) {
    History history("state");
    Eigen::VectorXd state(2);
    state << 1.0f, 2.0f;

    history.AddState(state, 0);
    ASSERT_FALSE(history.GetRecordedTimesteps().empty());
    ASSERT_FALSE(history.GetRecordedStates().empty());

    history.Clear();
    EXPECT_TRUE(history.GetRecordedTimesteps().empty());
    EXPECT_TRUE(history.GetRecordedStates().empty());
}

TEST_F(HistoryTest, HasPendingStateReturnsTrueWhenPending) {
    History history("total_overdose", HistoryMode::kAccumulated);
    Eigen::VectorXd state(2);
    state << 1.0f, 2.0f;

    EXPECT_FALSE(history.HasPendingState());
    history.AccumulateState(state);
    EXPECT_TRUE(history.HasPendingState());
}

TEST_F(HistoryTest, HasPendingStateReturnsFalseWhenNoPending) {
    History history("total_overdose", HistoryMode::kAccumulated);
    EXPECT_FALSE(history.HasPendingState());
}

TEST_F(HistoryTest, GetStateMap) {
    History history("state");
    Eigen::VectorXd state1(2);
    state1 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state1, 5);
    history.AddState(state2, 10);

    std::map<int, Eigen::VectorXd> expected_map = {{5, state1}, {10, state2}};

    auto state_map = history.GetStateMap();
    ASSERT_EQ(state_map.size(), expected_map.size());
    for (const auto &[timestep, expected_state] : expected_map) {
        ASSERT_TRUE(state_map.find(timestep) != state_map.end());
        EXPECT_TRUE(state_map[timestep].isApprox(expected_state));
    }
}

TEST_F(HistoryTest, GetRecordedTimesteps) {
    History history("state");
    Eigen::VectorXd state1(2);
    state1 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state1, 5);
    history.AddState(state2, 10);

    std::vector<int> expected_timesteps = {5, 10};
    EXPECT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
}

TEST_F(HistoryTest, GetRecordedStates) {
    History history("state");
    Eigen::VectorXd state1(2);
    state1 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state1, 5);
    history.AddState(state2, 10);

    const auto &recorded_states = history.GetRecordedStates();
    ASSERT_EQ(recorded_states.size(), 2u);
    EXPECT_TRUE(recorded_states[0].isApprox(state1));
    EXPECT_TRUE(recorded_states[1].isApprox(state2));
}

TEST_F(HistoryTest, GetHistoryMode) {
    History history("state", HistoryMode::kSnapshot);
    EXPECT_EQ(history.GetHistoryMode(), HistoryMode::kSnapshot);

    History history2("total_overdose", HistoryMode::kAccumulated);
    EXPECT_EQ(history2.GetHistoryMode(), HistoryMode::kAccumulated);
}

TEST_F(HistoryTest, GetPendingState) {
    History history("total_overdose", HistoryMode::kAccumulated);
    Eigen::VectorXd state(2);
    state << 1.0f, 2.0f;

    EXPECT_TRUE(history.GetPendingState().size() == 0);
    history.AccumulateState(state);
    EXPECT_TRUE(history.GetPendingState().isApprox(state));
}

TEST_F(HistoryTest, GetLatestRecordedTimestep) {
    History history("state");
    EXPECT_EQ(history.GetLatestRecordedTimestep(), -1);

    Eigen::VectorXd state1(2);
    state1 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state1, 5);
    history.AddState(state2, 10);

    EXPECT_EQ(history.GetLatestRecordedTimestep(), 10);
}

TEST_F(HistoryTest, GetLatestRecordedTimestepOnEmptyHistoryReturnsNegativeOne) {
    History history("state");
    EXPECT_EQ(history.GetLatestRecordedTimestep(), -1);
    FlushAllLoggers();
    EXPECT_TRUE(
        FileContains(RESPOND_DEFAULT_LOG_FILE,
                     "GetLatestRecordedTimestep called on empty history"));
}

TEST_F(HistoryTest, GetName) {
    History history("custom_history");
    EXPECT_EQ(history.GetName(), "custom_history");
}

TEST_F(HistoryTest, GetStateAsVectorFillsGapsWithZeroVectors) {
    History history("state");
    Eigen::VectorXd state1(2);
    state1 << 1.0f, 2.0f;
    Eigen::VectorXd state2(2);
    state2 << 3.0f, 4.0f;

    history.AddState(state1, 0);
    history.AddState(state2, 3);

    std::vector<Eigen::VectorXd> expected_states = {
        state1, Eigen::VectorXd::Zero(2), Eigen::VectorXd::Zero(2), state2};

    auto state_vector = history.GetStateAsVector();
    ASSERT_EQ(state_vector.size(), expected_states.size());
    for (size_t i = 0; i < expected_states.size(); ++i) {
        EXPECT_TRUE(state_vector[i].isApprox(expected_states[i]));
    }
}

TEST_F(HistoryTest, GetStateAsVectorOnEmptyHistoryReturnsEmptyVector) {
    History history("state");
    auto state_vector = history.GetStateAsVector();
    EXPECT_TRUE(state_vector.empty());
    FlushAllLoggers();
    EXPECT_TRUE(FileContains(RESPOND_DEFAULT_LOG_FILE,
                             "GetStateAsVector called on empty history:"));
}

} // namespace testing
} // namespace respond