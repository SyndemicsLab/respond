////////////////////////////////////////////////////////////////////////////////
// File: respond_test.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/respond.hpp>

#include <iostream>

#include <gtest/gtest.h>

namespace respond {
namespace testing {
class RespondTest : public ::testing::Test {
public:
    std::unique_ptr<Model> markov;
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

TEST_F(RespondTest, RunOneStep) {
    markov->SetState(init_state);

    auto migr = TransitionFactory::CreateTransition("migration", "test_log");
    migr->AddTransitionMatrix(migration_pop);
    markov->AddTransition(migr);

    auto beha = TransitionFactory::CreateTransition("behavior", "test_log");
    beha->AddTransitionMatrix(behavior_trans);
    markov->AddTransition(beha);

    auto inte = TransitionFactory::CreateTransition("intervention", "test_log");
    inte->AddTransitionMatrix(intervention_trans);
    markov->AddTransition(inte);

    auto over = TransitionFactory::CreateTransition("overdose", "test_log");
    over->AddTransitionMatrix(overdose_prob);
    over->AddTransitionMatrix(fod_prob);
    markov->AddTransition(over);

    auto back =
        TransitionFactory::CreateTransition("background_death", "test_log");
    back->AddTransitionMatrix(background_death_prob);
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

} // namespace testing
} // namespace respond