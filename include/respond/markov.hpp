////////////////////////////////////////////////////////////////////////////////
// File: markov.hpp                                                           //
// Project: respond                                                           //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MARKOV_HPP_
#define RESPOND_MARKOV_HPP_

#include <memory>
#include <string>

#include <Eigen/Dense>

#include <respond/types.hpp>

namespace respond {
/// @brief A namespace for the respond model functionality.

using stamper =
    std::function<void(HistoryStamp &, Eigen::VectorXd, Eigen::VectorXd)>;

using transition_function = std::function<Eigen::VectorXd(
    Eigen::VectorXd &, const std::vector<Eigen::MatrixXd> &)>;
using transition = std::pair<transition_function, std::vector<Eigen::MatrixXd>>;

/// @brief Class describing the Respond model simulation.
class Markov {
public:
    /// @brief Default destructor for the Respond class.
    virtual ~Markov() = default;

    virtual void SetState(const Eigen::VectorXd &state_vector) = 0;
    virtual Eigen::VectorXd GetState() const = 0;

    virtual void SetTransitions(const std::vector<transition> &transitions) = 0;
    virtual std::vector<transition> GetTransitions() const = 0;

    virtual void AddTransition(const transition &transition) = 0;

    /// @brief Core function to Run the Respond model.
    /// @param data_loader DataLoader containing all the necessary data for the
    /// simulation.
    virtual void Run(const int &num_steps) = 0;

    /// @brief Get the History from the simulation.
    /// @return A history object with the results of the latest run.
    virtual HistoryOverTime GetRunResults() const = 0;

    /// @brief Factory method to create a Respond instance.
    /// @param log_name Name of the logger to write errors to.
    /// @return An instance of Respond.
    static std::unique_ptr<Markov>
    Create(const std::string &log_name = "console");
};
} // namespace respond
#endif // RESPOND_MARKOV_HPP_
