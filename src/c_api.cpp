////////////////////////////////////////////////////////////////////////////////
// File: c_api.cpp                                                            //
// Project: respond                                                           //
// Created Date: 2026-04-28                                                   //
// Author: GitHub Copilot                                                     //
// -----                                                                      //
// Last Modified: 2026-04-28                                                  //
// Modified By: GitHub Copilot                                                //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/c_api.h>

#include <memory>
#include <string>

#include <Eigen/Dense>

#include <respond/model.hpp>
#include <respond/simulation.hpp>
#include <respond/transition_factory.hpp>

struct respond_model_handle {
    std::unique_ptr<respond::Model> model;
};

struct respond_transition_handle {
    std::unique_ptr<respond::Transition> transition;
};

struct respond_simulation_handle {
    std::unique_ptr<respond::Simulation> simulation;
};

namespace {
thread_local std::string g_last_error;

respond_status SetError(const std::string &message,
                        respond_status code = RESPOND_STATUS_ERROR) {
    g_last_error = message;
    return code;
}

void ClearError() { g_last_error.clear(); }

respond_status CheckModel(const respond_model_handle *model) {
    if (model == nullptr || !model->model) {
        return SetError("Model handle is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }
    return RESPOND_STATUS_OK;
}

respond_status CheckTransition(const respond_transition_handle *transition) {
    if (transition == nullptr || !transition->transition) {
        return SetError("Transition handle is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }
    return RESPOND_STATUS_OK;
}

respond_status CheckSimulation(const respond_simulation_handle *sim) {
    if (sim == nullptr || !sim->simulation) {
        return SetError("Simulation handle is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }
    return RESPOND_STATUS_OK;
}

respond_status BuildVector(const double *data, size_t length,
                           Eigen::VectorXd *out_vector) {
    if (out_vector == nullptr) {
        return SetError("Output vector pointer is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }
    if (data == nullptr && length > 0) {
        return SetError("Input data pointer is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }

    *out_vector = Eigen::VectorXd::Zero(static_cast<Eigen::Index>(length));
    for (size_t i = 0; i < length; ++i) {
        (*out_vector)(static_cast<Eigen::Index>(i)) = data[i];
    }
    return RESPOND_STATUS_OK;
}

respond_status BuildMatrix(const double *data, size_t rows, size_t cols,
                           Eigen::MatrixXd *out_matrix) {
    if (out_matrix == nullptr) {
        return SetError("Output matrix pointer is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }
    if (data == nullptr && rows * cols > 0) {
        return SetError("Input data pointer is null.",
                        RESPOND_STATUS_INVALID_ARGUMENT);
    }

    *out_matrix =
        Eigen::MatrixXd::Zero(static_cast<Eigen::Index>(rows),
                              static_cast<Eigen::Index>(cols));

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            size_t idx = r * cols + c;
            (*out_matrix)(static_cast<Eigen::Index>(r),
                          static_cast<Eigen::Index>(c)) = data[idx];
        }
    }
    return RESPOND_STATUS_OK;
}
} // namespace

extern "C" {

const char *respond_get_last_error_message(void) {
    return g_last_error.c_str();
}

respond_status respond_model_create(const char *name, const char *log_name,
                                    respond_model_handle **out_model) {
    ClearError();
    try {
        if (out_model == nullptr) {
            return SetError("Output model pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        std::string model_name = (name == nullptr) ? "markov" : name;
        std::string logger_name = (log_name == nullptr) ? "console" : log_name;

        auto handle = std::make_unique<respond_model_handle>();
        handle->model = respond::Model::Create(model_name, logger_name);
        if (!handle->model) {
            return SetError("Model creation returned null.");
        }

        *out_model = handle.release();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_create.");
    }
}

void respond_model_destroy(respond_model_handle *model) { delete model; }

respond_status respond_model_set_state(respond_model_handle *model,
                                       const double *data, size_t length) {
    ClearError();
    try {
        auto status = CheckModel(model);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }

        Eigen::VectorXd state;
        status = BuildVector(data, length, &state);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }

        model->model->SetState(state);
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_set_state.");
    }
}

respond_status respond_model_get_state_size(const respond_model_handle *model,
                                            size_t *out_length) {
    ClearError();
    try {
        auto status = CheckModel(model);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        if (out_length == nullptr) {
            return SetError("Output length pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        *out_length = static_cast<size_t>(model->model->GetState().size());
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_get_state_size.");
    }
}

respond_status respond_model_get_state(const respond_model_handle *model,
                                       double *out_data,
                                       size_t out_data_length) {
    ClearError();
    try {
        auto status = CheckModel(model);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        if (out_data == nullptr) {
            return SetError("Output data pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        Eigen::VectorXd state = model->model->GetState();
        size_t needed = static_cast<size_t>(state.size());
        if (out_data_length < needed) {
            return SetError("Output buffer is too small.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        for (size_t i = 0; i < needed; ++i) {
            out_data[i] = state(static_cast<Eigen::Index>(i));
        }
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_get_state.");
    }
}

respond_status respond_model_run_transitions(respond_model_handle *model) {
    ClearError();
    try {
        auto status = CheckModel(model);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        model->model->RunTransitions();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_run_transitions.");
    }
}

respond_status respond_model_create_default_histories(
    respond_model_handle *model) {
    ClearError();
    try {
        auto status = CheckModel(model);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        model->model->CreateDefaultHistories();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError(
            "Unknown error in respond_model_create_default_histories.");
    }
}

respond_status respond_model_add_transition(
    respond_model_handle *model, const respond_transition_handle *transition) {
    ClearError();
    try {
        auto model_status = CheckModel(model);
        if (model_status != RESPOND_STATUS_OK) {
            return model_status;
        }

        auto transition_status = CheckTransition(transition);
        if (transition_status != RESPOND_STATUS_OK) {
            return transition_status;
        }

        model->model->AddTransition(transition->transition);
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_add_transition.");
    }
}

respond_status respond_model_clear_transitions(respond_model_handle *model) {
    ClearError();
    try {
        auto status = CheckModel(model);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        model->model->ClearTransitions();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_model_clear_transitions.");
    }
}

respond_status respond_transition_create(const char *type, const char *log_name,
                                         respond_transition_handle **out) {
    ClearError();
    try {
        if (out == nullptr) {
            return SetError("Output transition pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        std::string transition_type = (type == nullptr) ? "" : type;
        std::string logger_name = (log_name == nullptr) ? "console" : log_name;

        auto handle = std::make_unique<respond_transition_handle>();
        handle->transition =
            respond::TransitionFactory::CreateTransition(transition_type,
                                                         logger_name);
        if (!handle->transition) {
            return SetError("Transition creation returned null.");
        }

        *out = handle.release();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_transition_create.");
    }
}

void respond_transition_destroy(respond_transition_handle *transition) {
    delete transition;
}

respond_status respond_transition_add_matrix(respond_transition_handle *t,
                                             const double *data, size_t rows,
                                             size_t cols) {
    ClearError();
    try {
        auto status = CheckTransition(t);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }

        Eigen::MatrixXd matrix;
        status = BuildMatrix(data, rows, cols, &matrix);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }

        t->transition->AddTransitionMatrix(matrix);
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_transition_add_matrix.");
    }
}

respond_status respond_transition_add_vector(respond_transition_handle *t,
                                             const double *data,
                                             size_t length) {
    ClearError();
    try {
        auto status = CheckTransition(t);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }

        Eigen::VectorXd vector;
        status = BuildVector(data, length, &vector);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }

        t->transition->AddTransitionMatrix(vector);
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_transition_add_vector.");
    }
}

respond_status respond_transition_clear_matrices(respond_transition_handle *t) {
    ClearError();
    try {
        auto status = CheckTransition(t);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        t->transition->ClearTransitionMatrices();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_transition_clear_matrices.");
    }
}

respond_status respond_simulation_create(const char *log_name,
                                         respond_simulation_handle **out) {
    ClearError();
    try {
        if (out == nullptr) {
            return SetError("Output simulation pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        std::string logger_name = (log_name == nullptr) ? "console" : log_name;

        auto handle = std::make_unique<respond_simulation_handle>();
        handle->simulation = std::make_unique<respond::Simulation>(logger_name);
        *out = handle.release();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_simulation_create.");
    }
}

void respond_simulation_destroy(respond_simulation_handle *sim) { delete sim; }

respond_status respond_simulation_add_model(
    respond_simulation_handle *sim, const respond_model_handle *model) {
    ClearError();
    try {
        auto sim_status = CheckSimulation(sim);
        if (sim_status != RESPOND_STATUS_OK) {
            return sim_status;
        }

        auto model_status = CheckModel(model);
        if (model_status != RESPOND_STATUS_OK) {
            return model_status;
        }

        sim->simulation->AddModel(model->model);
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_simulation_add_model.");
    }
}

respond_status respond_simulation_run(respond_simulation_handle *sim) {
    ClearError();
    try {
        auto status = CheckSimulation(sim);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        sim->simulation->Run();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_simulation_run.");
    }
}

respond_status respond_simulation_clear_models(respond_simulation_handle *sim) {
    ClearError();
    try {
        auto status = CheckSimulation(sim);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        sim->simulation->ClearModels();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_simulation_clear_models.");
    }
}

respond_status
respond_simulation_get_model_count(const respond_simulation_handle *sim,
                                   size_t *out_count) {
    ClearError();
    try {
        auto status = CheckSimulation(sim);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        if (out_count == nullptr) {
            return SetError("Output count pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        *out_count = sim->simulation->GetModels().size();
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_simulation_get_model_count.");
    }
}

respond_status respond_simulation_get_model_state_size(
    const respond_simulation_handle *sim, size_t model_index,
    size_t *out_length) {
    ClearError();
    try {
        auto status = CheckSimulation(sim);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        if (out_length == nullptr) {
            return SetError("Output length pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        const auto &models = sim->simulation->GetModels();
        if (model_index >= models.size()) {
            return SetError("Model index out of range.",
                            RESPOND_STATUS_OUT_OF_RANGE);
        }

        *out_length =
            static_cast<size_t>(models[model_index]->GetState().size());
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError(
            "Unknown error in respond_simulation_get_model_state_size.");
    }
}

respond_status respond_simulation_get_model_state(
    const respond_simulation_handle *sim, size_t model_index, double *out_data,
    size_t out_data_length) {
    ClearError();
    try {
        auto status = CheckSimulation(sim);
        if (status != RESPOND_STATUS_OK) {
            return status;
        }
        if (out_data == nullptr) {
            return SetError("Output data pointer is null.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        const auto &models = sim->simulation->GetModels();
        if (model_index >= models.size()) {
            return SetError("Model index out of range.",
                            RESPOND_STATUS_OUT_OF_RANGE);
        }

        Eigen::VectorXd state = models[model_index]->GetState();
        size_t needed = static_cast<size_t>(state.size());
        if (out_data_length < needed) {
            return SetError("Output buffer is too small.",
                            RESPOND_STATUS_INVALID_ARGUMENT);
        }

        for (size_t i = 0; i < needed; ++i) {
            out_data[i] = state(static_cast<Eigen::Index>(i));
        }
        return RESPOND_STATUS_OK;
    } catch (const std::exception &e) {
        return SetError(e.what());
    } catch (...) {
        return SetError("Unknown error in respond_simulation_get_model_state.");
    }
}

} // extern "C"
