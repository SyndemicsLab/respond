////////////////////////////////////////////////////////////////////////////////
// File: Simulation.hpp                                                       //
// Project: model                                                             //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MODEL_SIMULATION_HPP_
#define RESPOND_MODEL_SIMULATION_HPP_

#include <memory>

#include <respond/data_ops/cost_loader.hpp>
#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/utility_loader.hpp>

namespace respond {
/// @brief A namespace for the respond model functionality.
namespace model {

/// @brief Class describing the Respond model simulation.
class Respond {
public:
    /// @brief Default destructor for the Respond class.
    virtual ~Respond() = default;

    /// @brief Core function to Run the Respond model.
    /// @param data_loader DataLoader containing all the necessary data for the
    /// simulation.
    virtual void Run(const data_ops::DataLoader &data_loader) = 0;

    /// @brief Get the History from the simulation.
    /// @return A history object with the results of the latest run.
    virtual data_ops::History GetHistory() const = 0;

    /// @brief Factory method to create a Respond instance.
    /// @param log_name Name of the logger to write errors to.
    /// @return An instance of Respond.
    static std::unique_ptr<Respond>
    Create(const std::string &log_name = "console");
};
} // namespace model
} // namespace respond
#endif // RESPOND_MODEL_SIMULATION_HPP_
