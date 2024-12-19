#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <synmodels/data/RespondDataStore.hpp>

#include <memory>
#include <vector>

class RespondDataStoreTest : public ::testing::Test {
private:
    void RegisterLogger() {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

protected:
    std::shared_ptr<synmodels::data::RespondDataStore> test_data_store;
    void SetUp() override {
        RegisterLogger();
        test_data_store = synmodels::data::RespondDataStore::Create();
    }
    void TearDown() override { spdlog::drop("main"); }
};

// Getters/Setters Testing
TEST_F(RespondDataStoreTest, Duration) {
    int duration = 1234;
    test_data_store->SetDuration(duration);
    ASSERT_EQ(test_data_store->GetDuration(), duration);
}

TEST_F(RespondDataStoreTest, StoreHistoryStatus) {
    bool store_history_status = true;
    test_data_store->SetStoreHistoryStatus(store_history_status);
    ASSERT_EQ(test_data_store->GetStoreHistoryStatus(), store_history_status);
    store_history_status = false;
    test_data_store->SetStoreHistoryStatus(store_history_status);
    ASSERT_EQ(test_data_store->GetStoreHistoryStatus(), store_history_status);
}

TEST_F(RespondDataStoreTest, HistoryTimestepsToStore) {
    std::vector<int> timesteps = {1, 2, 3, 5, 6, 7};
    test_data_store->SetHistoryTimestepsToStore(timesteps);
    ASSERT_EQ(test_data_store->GetHistoryTimestepsToStore(), timesteps);
}

TEST_F(RespondDataStoreTest, InitialCohortState) {
    Eigen::VectorXd vec(4);
    vec << 2, 4, 6, 8;
    test_data_store->SetInitialCohortState(vec);
    ASSERT_EQ(test_data_store->GetInitialCohortState(), vec);
}

TEST_F(RespondDataStoreTest, MigratingCohortState) {
    Eigen::VectorXd vec(4);
    int timestep = 4;
    vec << 2, 4, 6, 8;
    test_data_store->SetMigratingCohortState(vec, timestep);
    ASSERT_EQ(test_data_store->GetMigratingCohortState(timestep), vec);
}

TEST_F(RespondDataStoreTest, BehaviorTransitions) {
    Eigen::MatrixXd mat(2, 2);
    int timestep = 4;
    mat << 1, 2, 3, 4;
    test_data_store->SetBehaviorTransitions(mat, timestep);
    ASSERT_EQ(test_data_store->GetBehaviorTransitions(timestep), mat);
}

TEST_F(RespondDataStoreTest, InterventionTransitions) {
    Eigen::MatrixXd mat(2, 2);
    int timestep = 4;
    mat << 1, 2, 3, 4;
    test_data_store->SetInterventionTransitions(mat, timestep);
    ASSERT_EQ(test_data_store->GetInterventionTransitions(timestep), mat);
}

TEST_F(RespondDataStoreTest, BehaviorAfterInterventionTransitions) {
    Eigen::MatrixXd mat(2, 2);
    int timestep = 4;
    mat << 1, 2, 3, 4;
    test_data_store->SetBehaviorAfterInterventionTransitions(mat, timestep);
    ASSERT_EQ(
        test_data_store->GetBehaviorAfterInterventionTransitions(timestep),
        mat);
}

TEST_F(RespondDataStoreTest, OverdoseProbabilityState) {
    Eigen::VectorXd vec(4);
    int timestep = 4;
    vec << 2, 4, 6, 8;
    test_data_store->SetOverdoseProbabilityState(vec, timestep);
    ASSERT_EQ(test_data_store->GetOverdoseProbabilityState(timestep), vec);
}

TEST_F(RespondDataStoreTest, OverdoseBeingFatalProbabilityState) {
    Eigen::VectorXd vec(4);
    int timestep = 4;
    vec << 2, 4, 6, 8;
    test_data_store->SetOverdoseBeingFatalProbabilityState(vec, timestep);
    ASSERT_EQ(test_data_store->GetOverdoseBeingFatalProbabilityState(timestep),
              vec);
}

TEST_F(RespondDataStoreTest, StandardMortalityRatioState) {
    Eigen::VectorXd vec(4);
    int timestep = 4;
    vec << 2, 4, 6, 8;
    test_data_store->SetStandardMortalityRatioState(vec, timestep);
    ASSERT_EQ(test_data_store->GetStandardMortalityRatioState(timestep), vec);
}

TEST_F(RespondDataStoreTest, BackgroundMortalityState) {
    Eigen::VectorXd vec(4);
    int timestep = 4;
    vec << 2, 4, 6, 8;
    test_data_store->SetBackgroundMortalityState(vec, timestep);
    ASSERT_EQ(test_data_store->GetBackgroundMortalityState(timestep), vec);
}
