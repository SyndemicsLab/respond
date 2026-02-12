////////////////////////////////////////////////////////////////////////////////
// File: respond_test.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/respond.hpp>

#include <iostream>

#include <gtest/gtest.h>

namespace respond {
namespace testing {

std::unique_ptr<Transition> MakeTestTransition(const std::string &name,
                                               Eigen::MatrixXd matrix) {
    auto migr = TransitionFactory::CreateTransition(name, "test_log");
    migr->AddTransitionMatrix(matrix);
    return migr;
}

class RespondTest : public ::testing::Test {
public:
    std::unique_ptr<Model> markov;
    std::unique_ptr<Transition> transition;
    Eigen::Vector3d init_state;
    Eigen::Vector3d migration_pop;
    Eigen::Matrix3d intervention_trans;
    Eigen::Matrix3d behavior_trans;
    Eigen::Vector3d overdose_prob;
    Eigen::Vector3d fod_prob;
    Eigen::Vector3d background_death_prob;

protected:
    void SetUp() override {
        markov = Model::Create("markov", "test_logger");
        init_state << 1.3f, 1.1f, 1.8f;
        migration_pop << 0.0f, 0.0f, 0.0f;
        intervention_trans << 0.1f, 0.2f, 0.5f, 0.3f, 0.2f, 0.3f, 0.7f, 0.2f,
            0.3f;
        behavior_trans << 0.3f, 0.2f, 0.1f, 0.4f, 0.2f, 0.1f, 0.3f, 0.4f, 0.1f;
        overdose_prob << 0.01f, 0.01f, 0.02f;
        fod_prob << 0.01f, 0.01f, 0.01f;
        background_death_prob << 0.001f, 0.001f, 0.002f;
    }
    void TearDown() override { markov.reset(); }
};

TEST_F(RespondTest, RunTransitionsInModel) {
    markov->SetState(init_state);

    auto migr = MakeTestTransition("migration", migration_pop);
    markov->AddTransition(migr);

    auto beha = MakeTestTransition("behavior", behavior_trans);
    markov->AddTransition(beha);

    auto inte = MakeTestTransition("intervention", intervention_trans);
    markov->AddTransition(inte);

    auto over = MakeTestTransition("overdose", overdose_prob);
    over->AddTransitionMatrix(fod_prob);
    markov->AddTransition(over);

    auto back = MakeTestTransition("background_death", background_death_prob);
    markov->AddTransition(back);

    markov->RunTransitions();

    auto t_names = markov->GetTransitionNames();
    std::vector<std::string> expected = {"migration", "behavior",
                                         "intervention", "overdose",
                                         "background_death"};
    ASSERT_EQ(t_names, expected);

    Eigen::Vector3d final_state;
    final_state << 0.76715528791564891, 0.72320370216816077, 1.037712429738102;
    ASSERT_TRUE(markov->GetState().isApprox(final_state));
}

TEST_F(RespondTest, RunSimulationOneStep) {
    markov->CreateDefaultHistories();

    markov->SetState(init_state);

    auto migr = MakeTestTransition("migration", migration_pop);
    markov->AddTransition(migr);

    auto beha = MakeTestTransition("behavior", behavior_trans);
    markov->AddTransition(beha);

    auto inte = MakeTestTransition("intervention", intervention_trans);
    markov->AddTransition(inte);

    auto over = MakeTestTransition("overdose", overdose_prob);
    over->AddTransitionMatrix(fod_prob);
    markov->AddTransition(over);

    auto back = MakeTestTransition("background_death", background_death_prob);
    markov->AddTransition(back);

    Simulation sim("test_logger");
    sim.AddModel(markov);
    sim.Run();

    auto histories = sim.GetModelHistories();
    ASSERT_EQ(histories.size(), 1);

    auto mm_histories = histories[0];
    if (mm_histories.find("state") == mm_histories.end()) {
        FAIL() << "Unable to find the 'state' history.";
    }

    auto state_history = mm_histories.at("state");
    ASSERT_EQ(state_history.size(), 1);

    Eigen::Vector3d final_state;
    final_state << 0.76715528791564891, 0.72320370216816077, 1.037712429738102;
    ASSERT_TRUE(state_history[0].isApprox(final_state));
}

TEST_F(RespondTest, CreateDefaultHistories) {
    std::vector<std::string> expected = {
        "state", "total_overdose", "fatal_overdose", "intervention_admission",
        "background_death"};

    std::sort(expected.begin(), expected.end());

    markov->CreateDefaultHistories();
    std::vector<std::string> results;
    for (const auto &kv : markov->GetHistories()) {
        results.push_back(kv.first);
    }
    ASSERT_EQ(results, expected);
}

} // namespace testing
} // namespace respond