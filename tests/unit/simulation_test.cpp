////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/simulation.hpp>

#include <algorithm>
#include <memory>

#include <gtest/gtest.h>

#include "../mocks/model_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace respond {
namespace testing {
class SimulationTest : public ::testing::Test {
public:
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SimulationTest, ConstructGetLogger) {
    Simulation s;
    ASSERT_EQ(s.GetLogName(), "console");
}

TEST_F(SimulationTest, GetSetModel) {
    auto mock = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*mock, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));

    std::unique_ptr<Model> upmm = std::move(mock);
    Simulation s;
    s.AddModel(upmm);
    ASSERT_EQ(s.GetModels().size(), 1);
}

TEST_F(SimulationTest, ClearModels) {
    auto mock = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*mock, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));

    std::unique_ptr<Model> upmm = std::move(mock);
    Simulation s;
    s.AddModel(upmm);
    s.ClearModels();
    ASSERT_EQ(s.GetModels().size(), 0);
}

TEST_F(SimulationTest, GetModelNames) {
    auto mock = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    auto expected = "test_model_name";
    EXPECT_CALL(*cloned, GetModelName()).WillOnce(Return(expected));
    EXPECT_CALL(*mock, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));

    std::unique_ptr<Model> upmm = std::move(mock);
    Simulation s;
    s.AddModel(upmm);
    auto result = s.GetModelNames();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], expected);
}

TEST_F(SimulationTest, GetModelHistories) {
    auto mock = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();

    std::map<std::string, History> hv;
    History h("temp", "test_logger");
    Eigen::VectorXd state = Eigen::VectorXd(3);
    state << 1.0f, 2.0f, 3.0f;
    h.AddState(state);
    hv["temp"] = h;

    std::map<std::string, std::vector<Eigen::VectorXd>> h_map;
    h_map["temp"] = h.GetStateAsVector();

    std::vector<std::map<std::string, std::vector<Eigen::VectorXd>>> expected;
    expected.push_back(h_map);

    ON_CALL(*cloned, GetModelName()).WillByDefault(Return("temp_model"));

    EXPECT_CALL(*cloned, GetHistories()).WillOnce(Return(hv));
    ON_CALL(*mock, clone())
        .WillByDefault(Return(::testing::ByMove(std::move(cloned))));

    std::unique_ptr<Model> upmm = std::move(mock);
    Simulation s;
    s.AddModel(upmm);
    ASSERT_EQ(s.GetModelHistories(), expected);
}

TEST_F(SimulationTest, GetHistoryNames) {
    auto mock = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();

    std::string model_name = "temp_model";
    std::string history_name = "temp_history";

    std::map<std::string, History> hv;
    History h("temp", "test_logger");
    hv[history_name] = h;

    std::vector<std::pair<std::string, std::string>> expected = {
        {model_name, history_name}};

    ON_CALL(*cloned, GetModelName()).WillByDefault(Return(model_name));

    EXPECT_CALL(*cloned, GetHistories()).WillOnce(Return(hv));
    ON_CALL(*mock, clone())
        .WillByDefault(Return(::testing::ByMove(std::move(cloned))));

    std::unique_ptr<Model> upmm = std::move(mock);
    Simulation s;
    s.AddModel(upmm);
    ASSERT_EQ(s.GetModelHistoryNames(), expected);
}

TEST_F(SimulationTest, GetModelSparseHistories) {
    auto mock = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();

    std::map<std::string, History> hv;
    History h("temp", "test_logger");
    Eigen::VectorXd state0 = Eigen::VectorXd(2);
    state0 << 1.0f, 2.0f;
    Eigen::VectorXd state2 = Eigen::VectorXd(2);
    state2 << 3.0f, 4.0f;
    h.AddState(state0, 0);
    h.AddState(state2, 2);
    hv["temp"] = h;

    EXPECT_CALL(*cloned, GetHistories()).WillOnce(Return(hv));
    ON_CALL(*mock, clone())
        .WillByDefault(Return(::testing::ByMove(std::move(cloned))));

    std::unique_ptr<Model> upmm = std::move(mock);
    Simulation s;
    s.AddModel(upmm);

    const auto histories = s.GetModelSparseHistories();
    ASSERT_EQ(histories.size(), 1u);
    const auto &history = histories[0].at("temp");
    std::vector<int> expected_timesteps = {0, 2};
    ASSERT_EQ(history.GetRecordedTimesteps(), expected_timesteps);
    ASSERT_EQ(history.GetRecordedStates().size(), 2u);
    EXPECT_TRUE(history.GetRecordedStates()[0].isApprox(state0));
    EXPECT_TRUE(history.GetRecordedStates()[1].isApprox(state2));
}

} // namespace testing
} // namespace respond