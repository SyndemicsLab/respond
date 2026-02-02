////////////////////////////////////////////////////////////////////////////////
// File: markov.hpp                                                           //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MARKOV_HPP_
#define RESPOND_MARKOV_HPP_

#include <memory>
#include <string>

#include <Eigen/Dense>

#include <respond/types.hpp>

namespace respond {

/// @brief A helper class to hold Transitions
class Transition {
public:
    /// @brief A vector of matrices containing the transition matrices to
    /// multiply.
    std::vector<Eigen::MatrixXd> transition_matrices;

    void SetCallback(std::function<Eigen::VectorXd(
                         const Eigen::VectorXd &,
                         const std::vector<Eigen::MatrixXd> &, HistoryStamp &)>
                         cb) {
        _callback = std::move(cb);
    }

    Eigen::VectorXd Execute(const Eigen::VectorXd &a,
                            const std::vector<Eigen::MatrixXd> &b,
                            HistoryStamp &c) {
        if (_callback) {
            return _callback(a, b, c);
        }
        return a;
    }

private:
    /// @brief The callback function to apply.
    std::function<Eigen::VectorXd(const Eigen::VectorXd &,
                                  const std::vector<Eigen::MatrixXd> &,
                                  HistoryStamp &)>
        _callback;
};

/// @brief Class describing the Respond model simulation.
class Markov {
public:
    /// @brief Default destructor for the Respond class.
    virtual ~Markov() = default;

    /// @brief Setter for the state vector.
    /// @param state_vector The vector to provide as the model state.
    virtual void SetState(const Eigen::VectorXd &state_vector) = 0;

    /// @brief Getter for the state vector.
    /// @return The state vector.
    virtual Eigen::VectorXd GetState() const = 0;

    /// @brief Setter for the transition operations.
    /// @param transitions A vector of transition operations and matrices.
    virtual void SetTransitions(const std::vector<Transition> &transitions) = 0;

    /// @brief Getter for the transition operations.
    /// @return Vector of transition operations and matrices.
    virtual std::vector<Transition> GetTransitions() const = 0;

    /// @brief Add a single transition operation to the vector.
    /// @param transition A transition operation and the matrices.
    virtual void AddTransition(const Transition &transition) = 0;

    /// @brief Core function to Run the Markov model.
    /// @param num_steps The number of steps to run through the model.
    virtual void Run(const int &num_steps) = 0;

    /// @brief Get the History from the simulation.
    /// @return A history object with the results of the latest run.
    virtual HistoryOverTime GetRunResults() const = 0;

    /// @brief Getter for the logger name.
    /// @return The name of the logger.
    virtual std::string GetLoggerName() const = 0;

    virtual std::unique_ptr<Markov> clone() const = 0;

    /// @brief Factory method to create a Markov instance.
    /// @param log_name Name of the logger to write errors to.
    /// @return An instance of Markov.
    static std::unique_ptr<Markov>
    Create(const std::string &log_name = "console");
};
} // namespace respond
#endif // RESPOND_MARKOV_HPP_
