////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/simulation.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "../mocks/model_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

namespace respond {
namespace testing {
class SimulationTest : public ::testing::Test {
public:
protected:
    void SetUp() override {
        // Clear any existing loggers from previous tests
        spdlog::drop_all();

        // Create temporary log files for testing
        test_log_file_ = "/tmp/respond_test.log";
        shared_log_file_ = "/tmp/respond_shared.log";
        default_log_file_ = RESPOND_DEFAULT_LOG_FILE;

        // Remove test files if they exist
        std::remove(default_log_file_.c_str());
        std::remove(test_log_file_.c_str());
        std::remove(shared_log_file_.c_str());
    }
    void TearDown() override {
        // Clean up loggers
        spdlog::drop_all();

        // Remove test files
        std::remove(default_log_file_.c_str());
        std::remove(test_log_file_.c_str());
        std::remove(shared_log_file_.c_str());
    }

    std::string test_log_file_;
    std::string shared_log_file_;
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

TEST_F(SimulationTest, DefaultConstructor) {
    Simulation s;
    ASSERT_EQ(CreateFileLogger(RESPOND_DEFAULT_LOG, default_log_file_),
              CreationStatus::kExists);
}

TEST_F(SimulationTest, ConstructorWithLogName) {
    Simulation s("custom_log");
    ASSERT_EQ(CreateFileLogger("custom_log", default_log_file_),
              CreationStatus::kExists);
}

TEST_F(SimulationTest, ConstructorWithLogNameAndLogFile) {
    Simulation s("custom_log", test_log_file_);
    ASSERT_EQ(CreateFileLogger("custom_log", test_log_file_),
              CreationStatus::kExists);
}

TEST_F(SimulationTest, CreateNewModel) {
    Simulation s;
    std::string model_name = "test_model";
    std::string new_model_id = s.CreateNewModel(model_name);
    ASSERT_EQ(new_model_id, "1_" + model_name);
    ASSERT_EQ(s.GetModels().size(), 1);
}

TEST_F(SimulationTest, CreateMultipleModels) {
    Simulation s;
    std::string model_name1 = "test_model1";
    std::string model_name2 = "test_model2";
    std::string new_model_id1 = s.CreateNewModel(model_name1);
    std::string new_model_id2 = s.CreateNewModel(model_name2);
    ASSERT_EQ(new_model_id1, "1_" + model_name1);
    ASSERT_EQ(new_model_id2, "2_" + model_name2);
    ASSERT_EQ(s.GetModels().size(), 2);
}

TEST_F(SimulationTest, CreateModelWithExistingName) {
    Simulation s;
    std::string model_name = "test_model";
    std::string new_model_id1 = s.CreateNewModel(model_name);
    std::string new_model_id2 = s.CreateNewModel(model_name);
    ASSERT_EQ(new_model_id1, "1_" + model_name);
    ASSERT_EQ(new_model_id2, "2_" + model_name);
    ASSERT_EQ(s.GetModels().size(), 2);
}

TEST_F(SimulationTest, ClearModels) {
    Simulation s;
    std::string model_name = "test_model";
    s.CreateNewModel(model_name);
    ASSERT_EQ(s.GetModels().size(), 1);
    s.ClearModels();
    ASSERT_EQ(s.GetModels().size(), 0);
}

TEST_F(SimulationTest, AddModel) {
    Simulation s;
    auto mock_model = std::make_unique<NiceMock<MockModel>>();
    auto cloned_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*mock_model, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned_model))));

    s.AddModel(std::move(mock_model));
    ASSERT_EQ(s.GetModels().size(), 1);
}

TEST_F(SimulationTest, Run) {
    Simulation s;
    auto mock_model = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*cloned, RunTimesteps()).Times(1);
    EXPECT_CALL(*mock_model, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));
    s.AddModel(std::move(mock_model));
    s.Run();
}

TEST_F(SimulationTest, RunMultipleModels) {
    Simulation s;
    auto mock_model = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*cloned, RunTimesteps()).Times(1);
    EXPECT_CALL(*mock_model, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));
    s.AddModel(std::move(mock_model));

    auto mock_model2 = std::make_unique<NiceMock<MockModel>>();
    auto cloned2 = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*cloned2, RunTimesteps()).Times(1);
    EXPECT_CALL(*mock_model2, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned2))));
    s.AddModel(std::move(mock_model2));

    s.Run();
}

TEST_F(SimulationTest, GetModels) {
    Simulation s;
    auto mock_model = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*mock_model, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));
    s.AddModel(std::move(mock_model));

    const auto &models = s.GetModels();
    ASSERT_EQ(models.size(), 1);
}

TEST_F(SimulationTest, GetModelNames) {
    Simulation s;
    std::string model_name1 = "test_model1";
    std::string model_name2 = "test_model2";
    s.CreateNewModel(model_name1);
    s.CreateNewModel(model_name2);

    const auto &model_names = s.GetModelNames();
    ASSERT_EQ(model_names.size(), 2);
    ASSERT_EQ(model_names[0], model_name1);
    ASSERT_EQ(model_names[1], model_name2);
}

TEST_F(SimulationTest, GetModelHistories) {
    Simulation s;

    History history("history1");
    Eigen::VectorXd state0(2);
    state0 << 1.0, 2.0;
    Eigen::VectorXd state2(2);
    state2 << 3.0, 4.0;
    history.AddState(state0, 0);
    history.AddState(state2, 2);
    auto histories = std::map<std::string, History>{{"history1", history}};

    auto mock_model = std::make_unique<NiceMock<MockModel>>();
    auto cloned = std::make_unique<NiceMock<MockModel>>();
    auto *cloned_ptr = cloned.get();
    EXPECT_CALL(*cloned_ptr, GetHistories()).WillOnce(ReturnRef(histories));
    EXPECT_CALL(*mock_model, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned))));
    s.AddModel(std::move(mock_model));

    const auto model_histories = s.GetModelHistory(0);
    ASSERT_EQ(model_histories.size(), 1);
    ASSERT_EQ(model_histories.size(), 1);

    const auto history_it = model_histories.at("history1").GetStateAsVector();
    ASSERT_EQ(history_it.size(), 3);
    EXPECT_TRUE(history_it[0].isApprox(state0));
    EXPECT_TRUE(history_it[1].isApprox(Eigen::VectorXd::Zero(2)));
    EXPECT_TRUE(history_it[2].isApprox(state2));
}

TEST_F(SimulationTest, GetModelHistoryNames) {
    Simulation s;

    auto histories1 = std::map<std::string, History>{
        {"history1", History("history1")}, {"history2", History("history2")}};
    auto mock_model1 = std::make_unique<NiceMock<MockModel>>();
    auto cloned1 = std::make_unique<NiceMock<MockModel>>();
    auto *cloned1_ptr = cloned1.get();
    EXPECT_CALL(*cloned1_ptr, GetName()).WillRepeatedly(Return("model1"));
    EXPECT_CALL(*cloned1_ptr, GetHistories()).WillOnce(ReturnRef(histories1));
    EXPECT_CALL(*mock_model1, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned1))));
    s.AddModel(std::move(mock_model1));

    auto histories2 =
        std::map<std::string, History>{{"history3", History("history3")}};
    auto mock_model2 = std::make_unique<NiceMock<MockModel>>();
    auto cloned2 = std::make_unique<NiceMock<MockModel>>();
    auto *cloned2_ptr = cloned2.get();
    EXPECT_CALL(*cloned2_ptr, GetName()).WillRepeatedly(Return("model2"));
    EXPECT_CALL(*cloned2_ptr, GetHistories()).Times(0);
    EXPECT_CALL(*mock_model2, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned2))));
    s.AddModel(std::move(mock_model2));

    const auto history_names = s.GetModelHistoryNames(0);
    const std::vector<std::string> expected = {"history1", "history2"};

    ASSERT_EQ(history_names, expected);
}

} // namespace testing
} // namespace respond