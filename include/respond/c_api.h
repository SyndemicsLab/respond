////////////////////////////////////////////////////////////////////////////////
// File: c_api.h                                                              //
// Project: respond                                                           //
// Created Date: 2026-04-28                                                   //
// Author: GitHub Copilot                                                     //
// -----                                                                      //
// Last Modified: 2026-04-28                                                  //
// Modified By: GitHub Copilot                                                //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_C_API_H_
#define RESPOND_C_API_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct respond_model_handle respond_model_handle;
typedef struct respond_transition_handle respond_transition_handle;
typedef struct respond_simulation_handle respond_simulation_handle;

typedef enum respond_status {
    RESPOND_STATUS_OK = 0,
    RESPOND_STATUS_INVALID_ARGUMENT = 1,
    RESPOND_STATUS_OUT_OF_RANGE = 2,
    RESPOND_STATUS_ERROR = 3
} respond_status;

/// @brief Returns the most recent error message generated on the current
/// thread. The returned pointer is valid until the next C API call on the same
/// thread.
const char *respond_get_last_error_message(void);

/// @brief Creates a model handle.
respond_status respond_model_create(const char *name, const char *log_name,
                                    respond_model_handle **out_model);

/// @brief Destroys a model handle.
void respond_model_destroy(respond_model_handle *model);

/// @brief Sets the model state vector from a contiguous array.
respond_status respond_model_set_state(respond_model_handle *model,
                                       const double *data, size_t length);

/// @brief Returns the length of the current model state vector.
respond_status respond_model_get_state_size(const respond_model_handle *model,
                                            size_t *out_length);

/// @brief Copies the current model state vector into the provided output
/// buffer.
respond_status respond_model_get_state(const respond_model_handle *model,
                                       double *out_data,
                                       size_t out_data_length);

/// @brief Runs all transitions currently stored in the model.
respond_status respond_model_run_transitions(respond_model_handle *model);

/// @brief Creates the default history map for this model.
respond_status
respond_model_create_default_histories(respond_model_handle *model);

/// @brief Adds a transition to the model. The transition is cloned by the
/// model and ownership is not transferred.
respond_status
respond_model_add_transition(respond_model_handle *model,
                             const respond_transition_handle *transition);

/// @brief Removes all transitions from the model.
respond_status respond_model_clear_transitions(respond_model_handle *model);

/// @brief Creates a transition handle from a transition type.
respond_status respond_transition_create(const char *type, const char *log_name,
                                         respond_transition_handle **out);

/// @brief Destroys a transition handle.
void respond_transition_destroy(respond_transition_handle *transition);

/// @brief Adds a matrix to a transition from contiguous row-major data.
respond_status respond_transition_add_matrix(respond_transition_handle *t,
                                             const double *data, size_t rows,
                                             size_t cols);

/// @brief Adds a vector to a transition from contiguous data.
respond_status respond_transition_add_vector(respond_transition_handle *t,
                                             const double *data, size_t length);

/// @brief Clears all stored transition matrices.
respond_status respond_transition_clear_matrices(respond_transition_handle *t);

/// @brief Creates a simulation handle.
respond_status respond_simulation_create(const char *log_name,
                                         respond_simulation_handle **out);

/// @brief Destroys a simulation handle.
void respond_simulation_destroy(respond_simulation_handle *sim);

/// @brief Adds a model to the simulation. The model is cloned by the
/// simulation and ownership is not transferred.
respond_status respond_simulation_add_model(respond_simulation_handle *sim,
                                            const respond_model_handle *model);

/// @brief Executes one simulation step across all models.
respond_status respond_simulation_run(respond_simulation_handle *sim);

/// @brief Clears all models from the simulation.
respond_status respond_simulation_clear_models(respond_simulation_handle *sim);

/// @brief Gets the number of models in a simulation.
respond_status
respond_simulation_get_model_count(const respond_simulation_handle *sim,
                                   size_t *out_count);

/// @brief Gets the state vector length for a model within a simulation.
respond_status
respond_simulation_get_model_state_size(const respond_simulation_handle *sim,
                                        size_t model_index, size_t *out_length);

/// @brief Copies a model state vector from a simulation into the provided
/// output buffer.
respond_status
respond_simulation_get_model_state(const respond_simulation_handle *sim,
                                   size_t model_index, double *out_data,
                                   size_t out_data_length);

#ifdef __cplusplus
}
#endif

#endif // RESPOND_C_API_H_
