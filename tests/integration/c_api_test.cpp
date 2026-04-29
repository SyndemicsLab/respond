////////////////////////////////////////////////////////////////////////////////
// File: c_api_test.cpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-04-28                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/c_api.h>

#include <cmath>
#include <cstring>

#include <gtest/gtest.h>

namespace respond {
namespace testing {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static respond_model_handle *BuildModelWithTransitions() {
    respond_model_handle *model = nullptr;
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_model_create("markov", "test_logger", &model));

    // init state
    double init_state[] = {1.3, 1.1, 1.8};
    EXPECT_EQ(RESPOND_STATUS_OK, respond_model_set_state(model, init_state, 3));

    // migration (vector)
    respond_transition_handle *migr = nullptr;
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_create("migration", "test_logger", &migr));
    double migration_pop[] = {0.0, 0.0, 0.0};
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_add_vector(migr, migration_pop, 3));
    EXPECT_EQ(RESPOND_STATUS_OK, respond_model_add_transition(model, migr));
    respond_transition_destroy(migr);

    // behavior (3x3 matrix, row-major)
    respond_transition_handle *beha = nullptr;
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_create("behavior", "test_logger", &beha));
    double behavior_trans[] = {0.3, 0.2, 0.1, 0.4, 0.2, 0.1, 0.3, 0.4, 0.1};
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_add_matrix(beha, behavior_trans, 3, 3));
    EXPECT_EQ(RESPOND_STATUS_OK, respond_model_add_transition(model, beha));
    respond_transition_destroy(beha);

    // intervention (3x3 matrix, row-major)
    respond_transition_handle *inte = nullptr;
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_create("intervention", "test_logger", &inte));
    double intervention_trans[] = {0.1, 0.2, 0.5, 0.3, 0.2, 0.3, 0.7, 0.2, 0.3};
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_add_matrix(inte, intervention_trans, 3, 3));
    EXPECT_EQ(RESPOND_STATUS_OK, respond_model_add_transition(model, inte));
    respond_transition_destroy(inte);

    // overdose (two vectors: rates then fatality rates)
    respond_transition_handle *over = nullptr;
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_create("overdose", "test_logger", &over));
    double overdose_prob[] = {0.01, 0.01, 0.02};
    double fod_prob[] = {0.01, 0.01, 0.01};
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_add_vector(over, overdose_prob, 3));
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_add_vector(over, fod_prob, 3));
    EXPECT_EQ(RESPOND_STATUS_OK, respond_model_add_transition(model, over));
    respond_transition_destroy(over);

    // background_death (vector)
    respond_transition_handle *back = nullptr;
    EXPECT_EQ(RESPOND_STATUS_OK, respond_transition_create(
                                     "background_death", "test_logger", &back));
    double background_death_prob[] = {0.001, 0.001, 0.002};
    EXPECT_EQ(RESPOND_STATUS_OK,
              respond_transition_add_vector(back, background_death_prob, 3));
    EXPECT_EQ(RESPOND_STATUS_OK, respond_model_add_transition(model, back));
    respond_transition_destroy(back);

    return model;
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(CApiTest, RunTransitionsProducesExpectedState) {
    respond_model_handle *model = BuildModelWithTransitions();
    ASSERT_NE(nullptr, model);

    ASSERT_EQ(RESPOND_STATUS_OK, respond_model_run_transitions(model));

    size_t state_size = 0;
    ASSERT_EQ(RESPOND_STATUS_OK,
              respond_model_get_state_size(model, &state_size));
    ASSERT_EQ(3u, state_size);

    double out[3] = {};
    ASSERT_EQ(RESPOND_STATUS_OK, respond_model_get_state(model, out, 3));

    const double expected[] = {0.76715528791564891, 0.72320370216816077,
                               1.037712429738102};
    // Tolerance accounts for float-literal precision in the reference C++ test
    // vs. the double-precision input values used through the C ABI.
    const double tol = 1e-6;
    for (int i = 0; i < 3; ++i) {
        EXPECT_NEAR(expected[i], out[i], tol)
            << "State mismatch at index " << i;
    }

    respond_model_destroy(model);
}

TEST(CApiTest, SimulationOneStep) {
    respond_model_handle *model = BuildModelWithTransitions();
    ASSERT_NE(nullptr, model);

    ASSERT_EQ(RESPOND_STATUS_OK, respond_model_create_default_histories(model));

    respond_simulation_handle *sim = nullptr;
    ASSERT_EQ(RESPOND_STATUS_OK,
              respond_simulation_create("test_logger", &sim));
    ASSERT_EQ(RESPOND_STATUS_OK, respond_simulation_add_model(sim, model));
    ASSERT_EQ(RESPOND_STATUS_OK, respond_simulation_run(sim));

    size_t model_count = 0;
    ASSERT_EQ(RESPOND_STATUS_OK,
              respond_simulation_get_model_count(sim, &model_count));
    ASSERT_EQ(1u, model_count);

    size_t state_size = 0;
    ASSERT_EQ(RESPOND_STATUS_OK,
              respond_simulation_get_model_state_size(sim, 0, &state_size));
    ASSERT_EQ(3u, state_size);

    respond_simulation_destroy(sim);
    respond_model_destroy(model);
}

TEST(CApiTest, NullModelReturnsInvalidArgument) {
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_model_run_transitions(nullptr));
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_model_set_state(nullptr, nullptr, 0));

    size_t sz = 0;
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_model_get_state_size(nullptr, &sz));
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_model_get_state(nullptr, nullptr, 0));
}

TEST(CApiTest, NullTransitionReturnsInvalidArgument) {
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_transition_add_vector(nullptr, nullptr, 0));
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_transition_add_matrix(nullptr, nullptr, 0, 0));
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_transition_clear_matrices(nullptr));
}

TEST(CApiTest, GetStateSizeBufferTooSmall) {
    respond_model_handle *model = nullptr;
    ASSERT_EQ(RESPOND_STATUS_OK,
              respond_model_create("markov", "test_logger", &model));

    double init_state[] = {1.3, 1.1, 1.8};
    ASSERT_EQ(RESPOND_STATUS_OK, respond_model_set_state(model, init_state, 3));

    double out[2] = {};
    EXPECT_EQ(RESPOND_STATUS_INVALID_ARGUMENT,
              respond_model_get_state(model, out, 2));

    respond_model_destroy(model);
}

TEST(CApiTest, InvalidTransitionTypeReturnsError) {
    respond_transition_handle *t = nullptr;
    respond_status s =
        respond_transition_create("not_a_real_transition", "test_logger", &t);
    EXPECT_EQ(RESPOND_STATUS_ERROR, s);
    EXPECT_EQ(nullptr, t);
    EXPECT_NE(nullptr, respond_get_last_error_message());
    EXPECT_NE(0u, std::strlen(respond_get_last_error_message()));
}

TEST(CApiTest, ClearTransitionsAllowsReuse) {
    respond_model_handle *model = BuildModelWithTransitions();
    ASSERT_NE(nullptr, model);

    ASSERT_EQ(RESPOND_STATUS_OK, respond_model_clear_transitions(model));
    ASSERT_EQ(RESPOND_STATUS_OK, respond_model_run_transitions(model));

    respond_model_destroy(model);
}

} // namespace testing
} // namespace respond
