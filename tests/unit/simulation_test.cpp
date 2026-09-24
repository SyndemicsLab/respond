////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-08-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/simulation.hpp>

#include <algorithm>
#include <atomic>
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

TEST_F(SimulationTest, RunRejectsSimulationWithNoModels) {
    Simulation simulation;

    try {
        simulation.Run();
        FAIL() << "Expected Run() to reject a simulation with no models.";
    } catch (const std::invalid_argument &error) {
        EXPECT_STREQ(error.what(),
                     "Error attempting to run simulation with no models.");
    }
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

TEST_F(SimulationTest, StoresExecutionConfig) {
    ExecutionConfig config;
    config.total_threads = 4;
    config.eigen_threads = 1;
    config.run_models_concurrently = true;

    Simulation s("custom_log", test_log_file_, config);
    EXPECT_EQ(s.GetExecutionConfig().total_threads, 4);
    EXPECT_EQ(s.GetExecutionConfig().eigen_threads, 1);
    EXPECT_TRUE(s.GetExecutionConfig().run_models_concurrently);

    ExecutionConfig updated;
    updated.total_threads = 2;
    updated.eigen_threads = 2;
    s.SetExecutionConfig(updated);

    EXPECT_EQ(s.GetExecutionConfig().total_threads, 2);
    EXPECT_EQ(s.GetExecutionConfig().eigen_threads, 2);
    EXPECT_FALSE(s.GetExecutionConfig().run_models_concurrently);
}

TEST_F(SimulationTest, PreservesExecutionConfigWhenCopied) {
    ExecutionConfig config;
    config.total_threads = 4;
    config.eigen_threads = 1;
    config.run_models_concurrently = true;

    Simulation original("custom_log", test_log_file_, config);
    Simulation copy(original);

    EXPECT_EQ(copy.GetExecutionConfig().total_threads, 4);
    EXPECT_EQ(copy.GetExecutionConfig().eigen_threads, 1);
    EXPECT_TRUE(copy.GetExecutionConfig().run_models_concurrently);
}

TEST_F(SimulationTest, StoresRuntimeConfig) {
    RuntimeConfig config;
    config.execution.total_threads = 4;
    config.logging.logger_name = "runtime_simulation";
    config.logging.file_path = test_log_file_;

    Simulation s(config);

    EXPECT_EQ(s.GetRuntimeConfig().execution.total_threads, 4);
    EXPECT_EQ(s.GetRuntimeConfig().logging.logger_name,
              "runtime_simulation");
    EXPECT_EQ(s.GetRuntimeConfig().logging.file_path, test_log_file_);
}

TEST_F(SimulationTest, CreateNewModel) {
    Simulation s;
    std::string model_name = "test_model";
    auto new_model = s.CreateNewModel(model_name);
    ASSERT_NE(new_model, nullptr);
    ASSERT_EQ(new_model->GetName(), model_name);
    ASSERT_EQ(s.GetModels().size(), 1);
}

TEST_F(SimulationTest, EditsCreatedModelCopyBeforeManagingIt) {
    Simulation simulation;
    auto model_copy = simulation.CreateNewModel("test_model");
    model_copy->SetFinalTimestep(12);

    EXPECT_EQ(simulation[0]->GetFinalTimestep(), -1);

    simulation[0] = *model_copy;

    EXPECT_EQ(simulation[0]->GetFinalTimestep(), 12);
}

TEST_F(SimulationTest, CreateMultipleModels) {
    Simulation s;
    std::string model_name1 = "test_model1";
    std::string model_name2 = "test_model2";
    auto new_model1 = s.CreateNewModel(model_name1);
    auto new_model2 = s.CreateNewModel(model_name2);
    ASSERT_NE(new_model1, nullptr);
    ASSERT_NE(new_model2, nullptr);
    ASSERT_EQ(new_model1->GetName(), model_name1);
    ASSERT_EQ(new_model2->GetName(), model_name2);
    ASSERT_EQ(s.GetModels().size(), 2);
}

TEST_F(SimulationTest, CreateModelWithExistingName) {
    Simulation s;
    std::string model_name = "test_model";
    auto new_model1 = s.CreateNewModel(model_name);
    auto new_model2 = s.CreateNewModel(model_name);
    ASSERT_NE(new_model1, nullptr);
    ASSERT_NE(new_model2, nullptr);
    ASSERT_EQ(new_model1->GetName(), model_name);
    ASSERT_EQ(new_model2->GetName(), model_name);
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

TEST_F(SimulationTest, MoveAssignmentReplacesDestinationModels) {
    Simulation source;
    source.CreateNewModel("source_model");

    Simulation destination;
    destination.CreateNewModel("destination_model");
    destination = std::move(source);

    ASSERT_EQ(destination.GetModelNames(),
              std::vector<std::string>{"source_model"});
    ASSERT_TRUE(source.GetModels().empty());
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

TEST_F(SimulationTest, AddNullModelThrows) {
    Simulation s;
    std::unique_ptr<Model> null_model;

    EXPECT_THROW(s.AddModel(null_model), std::invalid_argument);
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

TEST_F(SimulationTest, RunsModelsConcurrentlyWhenEnabled) {
    ExecutionConfig config;
    config.total_threads = 2;
    config.eigen_threads = 1;
    config.run_models_concurrently = true;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});

    std::atomic<int> entered{0};
    std::atomic<int> maximum_active{0};
    auto run_model = [&]() {
        const int active = entered.fetch_add(1) + 1;
        int observed_maximum = maximum_active.load();
        while (active > observed_maximum &&
               !maximum_active.compare_exchange_weak(observed_maximum,
                                                     active)) {
        }
        while (entered.load() < 2) {
            std::this_thread::yield();
        }
    };

    auto first_source = std::make_unique<NiceMock<MockModel>>();
    auto first_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*first_model, RunTimesteps()).WillOnce(run_model);
    EXPECT_CALL(*first_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(first_model))));
    simulation.AddModel(std::move(first_source));

    auto second_source = std::make_unique<NiceMock<MockModel>>();
    auto second_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*second_model, RunTimesteps()).WillOnce(run_model);
    EXPECT_CALL(*second_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(second_model))));
    simulation.AddModel(std::move(second_source));

    simulation.Run();

    EXPECT_EQ(entered.load(), 2);
    EXPECT_EQ(maximum_active.load(), 2);
}

TEST_F(SimulationTest, TotalThreadsOneRunsModelsSequentially) {
    ExecutionConfig config;
    config.total_threads = 1;
    config.run_models_concurrently = true;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});
    std::atomic<int> active{0};
    std::atomic<int> maximum_active{0};

    auto run_model = [&]() {
        const int current = active.fetch_add(1) + 1;
        maximum_active.store(std::max(maximum_active.load(), current));
        active.fetch_sub(1);
    };

    for (int index = 0; index < 2; ++index) {
        auto source = std::make_unique<NiceMock<MockModel>>();
        auto model = std::make_unique<NiceMock<MockModel>>();
        EXPECT_CALL(*model, RunTimesteps()).WillOnce(run_model);
        EXPECT_CALL(*source, clone())
            .WillOnce(Return(::testing::ByMove(std::move(model))));
        simulation.AddModel(std::move(source));
    }

    simulation.Run();

    EXPECT_EQ(maximum_active.load(), 1);
}

TEST_F(SimulationTest, AllowsEigenThreadsWhenConcurrencyFallsBackToSequential) {
    ExecutionConfig config;
    config.eigen_threads = 2;
    config.run_models_concurrently = false;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});

    for (int index = 0; index < 2; ++index) {
        auto source = std::make_unique<NiceMock<MockModel>>();
        auto model = std::make_unique<NiceMock<MockModel>>();
        EXPECT_CALL(*model, RunTimesteps()).Times(1);
        EXPECT_CALL(*source, clone())
            .WillOnce(Return(::testing::ByMove(std::move(model))));
        simulation.AddModel(std::move(source));
    }

    EXPECT_NO_THROW(simulation.Run());
}

TEST_F(SimulationTest, AllowsEigenThreadsForSingleConcurrentModel) {
    ExecutionConfig config;
    config.eigen_threads = 2;
    config.run_models_concurrently = true;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});

    auto source = std::make_unique<NiceMock<MockModel>>();
    auto model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*model, RunTimesteps()).Times(1);
    EXPECT_CALL(*source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(model))));
    simulation.AddModel(std::move(source));

    EXPECT_NO_THROW(simulation.Run());
}

TEST_F(SimulationTest, AppliesUpdatedEigenThreadsWhenRunning) {
    const int previous_eigen_threads = Eigen::nbThreads();
    ExecutionConfig config;
    config.eigen_threads = 2;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});

    auto source = std::make_unique<NiceMock<MockModel>>();
    auto model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*model, RunTimesteps()).Times(1);
    EXPECT_CALL(*source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(model))));
    simulation.AddModel(std::move(source));

    Eigen::setNbThreads(2);
    config.eigen_threads = 1;
    simulation.SetExecutionConfig(config);
    simulation.Run();

    EXPECT_EQ(Eigen::nbThreads(), 1);
    Eigen::setNbThreads(previous_eigen_threads);
}

TEST_F(SimulationTest, RejectsConcurrentEigenOversubscription) {
    ExecutionConfig config;
    config.eigen_threads = 2;
    config.run_models_concurrently = true;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});

    auto first_source = std::make_unique<NiceMock<MockModel>>();
    auto first_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*first_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(first_model))));
    simulation.AddModel(std::move(first_source));

    auto second_source = std::make_unique<NiceMock<MockModel>>();
    auto second_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*second_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(second_model))));
    simulation.AddModel(std::move(second_source));

    EXPECT_THROW(simulation.Run(), std::invalid_argument);
}

TEST_F(SimulationTest, RethrowsWorkerExceptionAfterJoining) {
    ExecutionConfig config;
    config.total_threads = 2;
    config.run_models_concurrently = true;
    Simulation simulation(RuntimeConfig{config, LoggingConfig{}});
    std::atomic<int> completed{0};

    auto throwing_source = std::make_unique<NiceMock<MockModel>>();
    auto throwing_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*throwing_model, RunTimesteps())
        .WillOnce(::testing::Throw(std::runtime_error("worker failure")));
    EXPECT_CALL(*throwing_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(throwing_model))));
    simulation.AddModel(std::move(throwing_source));

    auto completing_source = std::make_unique<NiceMock<MockModel>>();
    auto completing_model = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*completing_model, RunTimesteps())
        .WillOnce([&]() { completed.fetch_add(1); });
    EXPECT_CALL(*completing_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(completing_model))));
    simulation.AddModel(std::move(completing_source));

    EXPECT_THROW(simulation.Run(), std::runtime_error);
    EXPECT_EQ(completed.load(), 1);
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

TEST_F(SimulationTest, IndexOperatorMutableAccessCallsModelMethods) {
    Simulation s;

    auto source_model_mock = std::make_unique<NiceMock<MockModel>>();
    auto *source_model_ptr = source_model_mock.get();
    std::unique_ptr<Model> source_model = std::move(source_model_mock);
    auto stored_model = std::make_unique<NiceMock<MockModel>>();
    auto *stored_model_ptr = stored_model.get();
    EXPECT_CALL(*stored_model_ptr, SetFinalTimestep(52)).Times(1);
    EXPECT_CALL(*source_model_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(stored_model)))));

    s.AddModel(source_model);
    s[0]->SetFinalTimestep(52);
}

TEST_F(SimulationTest, IndexOperatorConstAccessCallsModelMethods) {
    Simulation s;

    auto source_model_mock = std::make_unique<NiceMock<MockModel>>();
    auto *source_model_ptr = source_model_mock.get();
    std::unique_ptr<Model> source_model = std::move(source_model_mock);
    auto stored_model = std::make_unique<NiceMock<MockModel>>();
    auto *stored_model_ptr = stored_model.get();
    EXPECT_CALL(*stored_model_ptr, GetName()).WillOnce(Return("model_name"));
    EXPECT_CALL(*source_model_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(stored_model)))));

    s.AddModel(source_model);
    const Simulation &const_sim = s;
    ASSERT_EQ(const_sim[0].GetName(), "model_name");
}

TEST_F(SimulationTest, IndexOperatorThrowsOutOfRangeForInvalidIndex) {
    Simulation s;
    EXPECT_THROW((void)s[0], std::out_of_range);
}

TEST_F(SimulationTest, IndexOperatorAssignmentFromModelClonesSourceModel) {
    Simulation s;

    auto initial_source_mock = std::make_unique<NiceMock<MockModel>>();
    auto *initial_source_ptr = initial_source_mock.get();
    std::unique_ptr<Model> initial_source = std::move(initial_source_mock);
    auto initial_stored = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*initial_source_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(initial_stored)))));
    s.AddModel(initial_source);

    auto replacement_source = std::make_unique<NiceMock<MockModel>>();
    auto replacement_clone = std::make_unique<NiceMock<MockModel>>();
    auto *replacement_clone_ptr = replacement_clone.get();
    EXPECT_CALL(*replacement_clone_ptr, SetFinalTimestep(11)).Times(1);
    EXPECT_CALL(*replacement_source, clone())
        .WillOnce(Return(::testing::ByMove(std::move(replacement_clone))));

    s[0] = *replacement_source;
    s[0]->SetFinalTimestep(11);
}

TEST_F(SimulationTest,
       IndexOperatorAssignmentFromUniquePtrClonesAndKeepsCallerOwnership) {
    Simulation s;

    auto initial_source_mock = std::make_unique<NiceMock<MockModel>>();
    auto *initial_source_ptr = initial_source_mock.get();
    std::unique_ptr<Model> initial_source = std::move(initial_source_mock);
    auto initial_stored = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*initial_source_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(initial_stored)))));
    s.AddModel(initial_source);

    auto replacement_source_mock = std::make_unique<NiceMock<MockModel>>();
    auto *replacement_source_ptr = replacement_source_mock.get();
    std::unique_ptr<Model> replacement_source =
        std::move(replacement_source_mock);
    auto replacement_clone = std::make_unique<NiceMock<MockModel>>();
    auto *replacement_clone_ptr = replacement_clone.get();
    EXPECT_CALL(*replacement_clone_ptr, SetFinalTimestep(17)).Times(1);
    EXPECT_CALL(*replacement_source_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(replacement_clone)))));

    const std::unique_ptr<Model> &replacement_ref = replacement_source;
    s[0] = replacement_ref;
    ASSERT_NE(replacement_source, nullptr);
    s[0]->SetFinalTimestep(17);
}

TEST_F(SimulationTest, IndexOperatorAssignmentFromNullUniquePtrThrows) {
    Simulation s;

    auto initial_source_mock = std::make_unique<NiceMock<MockModel>>();
    auto *initial_source_ptr = initial_source_mock.get();
    std::unique_ptr<Model> initial_source = std::move(initial_source_mock);
    auto initial_stored = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*initial_source_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(initial_stored)))));
    s.AddModel(initial_source);

    std::unique_ptr<Model> null_model;
    EXPECT_THROW(s[0] = null_model, std::invalid_argument);
}

TEST_F(SimulationTest, IndexOperatorAssignmentFromProxyClonesSourceSlot) {
    Simulation s;

    auto target_source_mock = std::make_unique<NiceMock<MockModel>>();
    auto *target_source_ptr = target_source_mock.get();
    std::unique_ptr<Model> target_source = std::move(target_source_mock);
    auto target_stored = std::make_unique<NiceMock<MockModel>>();
    EXPECT_CALL(*target_source_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(target_stored)))));
    s.AddModel(target_source);

    auto source_source_mock = std::make_unique<NiceMock<MockModel>>();
    auto *source_source_ptr = source_source_mock.get();
    std::unique_ptr<Model> source_source = std::move(source_source_mock);
    auto source_stored = std::make_unique<NiceMock<MockModel>>();
    auto replacement_clone = std::make_unique<NiceMock<MockModel>>();
    auto *replacement_clone_ptr = replacement_clone.get();
    EXPECT_CALL(*replacement_clone_ptr, SetFinalTimestep(23)).Times(1);
    EXPECT_CALL(*source_stored, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(replacement_clone)))));
    EXPECT_CALL(*source_source_ptr, clone())
        .WillOnce(Return(::testing::ByMove(
            std::unique_ptr<Model>(std::move(source_stored)))));
    s.AddModel(source_source);

    s[0] = s[1];
    s[0]->SetFinalTimestep(23);
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
    EXPECT_CALL(*cloned2_ptr, GetHistories()).WillOnce(ReturnRef(histories2));
    EXPECT_CALL(*mock_model2, clone())
        .WillOnce(Return(::testing::ByMove(std::move(cloned2))));
    s.AddModel(std::move(mock_model2));

    const auto history_names = s.GetModelHistoryNames(0);
    const std::vector<std::string> expected = {"history1", "history2"};
    ASSERT_EQ(history_names, expected);

    const auto history_names_two = s.GetModelHistoryNames(1);
    const std::vector<std::string> expected_two = {"history3"};
    ASSERT_EQ(history_names_two, expected_two);
}

} // namespace testing
} // namespace respond