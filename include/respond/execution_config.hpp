////////////////////////////////////////////////////////////////////////////////
// File: execution_config.hpp                                                 //
// Project: respond                                                           //
// Created Date: 2026-09-22                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_EXECUTION_CONFIG_HPP_
#define RESPOND_EXECUTION_CONFIG_HPP_

namespace respond {
/// @brief Controls how a Simulation distributes execution resources.
struct ExecutionConfig {
    /// @brief Total worker threads available for concurrent model execution.
    /// A value of 0 selects the implementation's automatic default.
    unsigned int total_threads = 0;

    /// @brief Eigen worker threads used within a model.
    /// Use 1 when models are executed concurrently to avoid oversubscription.
    unsigned int eigen_threads = 1;

    /// @brief Whether models may be executed concurrently by the simulation.
    /// When multiple models actually run in parallel, eigen_threads must be 1
    /// or less because Eigen's worker setting is process-global.
    bool run_models_concurrently = false;
};
} // namespace respond

#endif // RESPOND_EXECUTION_CONFIG_HPP_
