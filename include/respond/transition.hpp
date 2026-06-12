////////////////////////////////////////////////////////////////////////////////
// File: transition.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TRANSITION_HPP_
#define RESPOND_TRANSITION_HPP_

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include <respond/history.hpp>

namespace respond {

/// @brief Abstract base class representing a state transition operation.
/// Transitions apply transformation matrices to state vectors and update
/// history records. Subclasses define specific types of transitions (e.g.,
/// Markov, background death, behavior).
class Transition {
public:
    /// @brief Virtual destructor for proper polymorphic cleanup.
    virtual ~Transition() = default;

    /// @brief Executes this transition, applying it to a state vector.
    /// The input state is not modified; history records are updated with the
    /// transition effects.
    /// @param s The current state vector (not modified).
    /// @param h The history records to update (may be modified by this
    /// transition).
    /// @return The resulting state vector after applying this transition.
    virtual Eigen::VectorXd
    Execute(const Eigen::VectorXd &s,
            std::map<std::string, History> &h) const = 0;

    /// @brief Adds a transformation matrix to this transition.
    /// The matrix is stored for use during Execute() calls.
    /// @param m The transition matrix to add (not modified by this transition).
    virtual void AddTransitionMatrix(const Eigen::MatrixXd &m) = 0;

    /// @brief Retrieves the name/type of this transition.
    /// @return The transition's identifier as a string.
    virtual std::string GetTransitionName() const = 0;

    /// @brief Clears all stored transition matrices.
    virtual void ClearTransitionMatrices() = 0;

    /// @brief Retrieves the logger name used by this transition.
    /// @return The associated logger's name.
    virtual std::string GetLogName() const = 0;

    /// @brief Deleted copy constructor (transitions are non-copyable by public
    /// API).
    Transition(const Transition &) = delete;
    /// @brief Deleted copy assignment operator (transitions are non-copyable by
    /// public API).
    Transition &operator=(const Transition &) = delete;

    /// @brief Creates a deep copy of this transition.
    /// @return A unique_ptr to an independent copy of this transition.
    virtual std::unique_ptr<Transition> clone() const = 0;

protected:
    /// @brief Protected default constructor for subclass initialization.
    /// Not intended for direct public use.
    Transition() = default;
};
} // namespace respond

#endif