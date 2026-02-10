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

/// @brief A helper class to hold Transitions
class Transition {
public:
    virtual ~Transition() = default;
    // Run the execute function and return the final state. Do not edit the
    // parameter state, but do edit the history provided. Nothing in the
    // Transition object should change.
    virtual Eigen::VectorXd
    Execute(const Eigen::VectorXd &s,
            std::map<std::string, History> &h) const = 0;
    // Add a Transition Matrix to the set. We have no need to edit it once it's
    // been added, just use it. Thus, we don't need full ownership (reference)
    // and can accept the const type.
    virtual void AddTransitionMatrix(const Eigen::MatrixXd &m) = 0;
    // Get the name of the Transition. No need to edit the object and do not
    // need user to edit the name.
    virtual std::string GetTransitionName() const = 0;
    // Clear out all the stored Eigen::MatrixXd values
    virtual void ClearTransitionMatrices() = 0;
    // Get Log Name
    virtual std::string GetLogName() const = 0;
    // Clone
    Transition(const Transition &) = delete;
    Transition &operator=(const Transition &) = delete;
    virtual std::unique_ptr<Transition> clone() const = 0;

protected:
    // default constructor required for subclasses, but do not want people to
    // use this
    Transition() = default;
};
} // namespace respond

#endif