////////////////////////////////////////////////////////////////////////////////
// File: respond_test.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/respond.hpp>

#include <iostream>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

namespace respond {
namespace testing {

class RespondTest : public ::testing::Test {
public:
    Simulation sim;
    Eigen::Vector3d init_state;
    Eigen::Vector3d migration_pop;
    Eigen::Matrix3d intervention_trans;
    Eigen::Matrix3d behavior_trans;
    Eigen::Vector3d overdose_prob;
    Eigen::Vector3d fod_prob;
    Eigen::Vector3d background_death_prob;
    Eigen::Vector3d tolerance;

protected:
    void SetUp() override {
        // Clear any existing loggers from previous tests
        spdlog::drop_all();

        // Create temporary log files for testing
        default_log_file_ = RESPOND_DEFAULT_LOG_FILE;

        // Remove test files if they exist
        std::remove(default_log_file_.c_str());

        init_state << 1.3f, 1.1f, 1.8f;
        migration_pop << 0.0f, 0.0f, 0.0f;
        behavior_trans << 0.3f, 0.2f, 0.1f, 0.4f, 0.2f, 0.1f, 0.3f, 0.4f, 0.1f;
        intervention_trans << 0.1f, 0.2f, 0.5f, 0.3f, 0.2f, 0.3f, 0.7f, 0.2f,
            0.3f;
        overdose_prob << 0.01f, 0.01f, 0.02f;
        fod_prob << 0.01f, 0.01f, 0.01f;
        background_death_prob << 0.001f, 0.001f, 0.002f;

        tolerance << 1e-5, 1e-5, 1e-5;

        sim.CreateNewModel("markov");
        sim[0]->CreateDefaultHistories();
        sim[0]->SetState(init_state);
    }
    void TearDown() override {
        // Clean up loggers
        spdlog::drop_all();

        // Remove test files
        std::remove(default_log_file_.c_str());
    }

    std::string default_log_file_;

    Timestep CreateTestTimestep() {

        Timestep ts;
        ts.CreateTransition("migration");
        ts.AddMatrixToTransition("migration", migration_pop);

        ts.CreateTransition("behavior");
        ts.AddMatrixToTransition("behavior", behavior_trans);

        ts.CreateTransition("intervention");
        ts.AddMatrixToTransition("intervention", intervention_trans);

        ts.CreateTransition("overdose");
        ts.AddMatrixToTransition("overdose", overdose_prob);
        ts.AddMatrixToTransition("overdose", fod_prob);

        ts.CreateTransition("background_death");
        ts.AddMatrixToTransition("background_death", background_death_prob);
        return ts;
    }
};

TEST_F(RespondTest, RunSingleTimestep) {
    sim[0]->AddTimestep(CreateTestTimestep());
    sim.Run();
    Eigen::VectorXd result =
        sim.GetModelHistory(0).at("state").GetStateAsVector().back();

    Eigen::Vector3d final_state;
    final_state << 0.76715528791564891, 0.72320370216816077, 1.037712429738102;
    ASSERT_TRUE(result.isApprox(final_state));
}

TEST_F(RespondTest, RunSimulationTwoStep) {
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim.Run(2);

    auto state_history = sim.GetModelHistory(0).at("state").GetStateAsVector();
    // 2 because it carries the initial state and 2 steps
    ASSERT_EQ(state_history.size(), 3);

    Eigen::Vector3d final_state;
    ASSERT_TRUE(state_history[0].isApprox(init_state));

    final_state << 0.46999281, 0.44109648, 0.631613324;
    Eigen::Vector3d diff = (state_history[2] - final_state).cwiseAbs();
    ASSERT_TRUE((diff.array() <= tolerance.array()).all());
}

TEST_F(RespondTest, RunSimulationFiveStep) {
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim.SetDuration(5);
    sim.Run();

    auto state_history = sim.GetModelHistory(0).at("state").GetStateAsVector();
    // 6 because it carries the initial state and 5 timesteps
    ASSERT_EQ(state_history.size(), 6);

    Eigen::Vector3d final_state;
    ASSERT_TRUE(state_history[0].isApprox(init_state));

    final_state << 0.10714013, 0.10056269, 0.14400034;
    Eigen::Vector3d diff = (state_history[5] - final_state).cwiseAbs();
    ASSERT_TRUE((diff.array() <= tolerance.array()).all());
}

TEST_F(RespondTest, RunSimulationFiveStepWithDurationParameter) {
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim[0]->AddTimestep(CreateTestTimestep());
    sim.Run(5);

    auto state_history = sim.GetModelHistory(0).at("state").GetStateAsVector();
    // 6 because it carries the initial state and 5 timesteps
    ASSERT_EQ(state_history.size(), 6);

    Eigen::Vector3d final_state;
    ASSERT_TRUE(state_history[0].isApprox(init_state));
    final_state << 0.10714013, 0.10056269, 0.14400034;
    Eigen::Vector3d diff = (state_history[5] - final_state).cwiseAbs();
    ASSERT_TRUE((diff.array() <= tolerance.array()).all());
}

} // namespace testing
} // namespace respond