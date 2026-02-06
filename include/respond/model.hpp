////////////////////////////////////////////////////////////////////////////////
// File: model.hpp                                                            //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_MODEL_HPP_
#define RESPOND_MODEL_HPP_

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <respond/history.hpp>
#include <respond/transition.hpp>

namespace respond {
class Model {
public:
    virtual ~Model() = default;
    // anticipate making a copy of the vector
    virtual void SetState(const Eigen::VectorXd &) = 0;
    // return const & to limit to observation of the state
    virtual Eigen::VectorXd GetState() const = 0;
    // manipulate the state vector
    virtual void RunTransitions() = 0;
    // assume ownership of the Transition
    virtual void AddTransition(const std::unique_ptr<Transition> &t) = 0;
    // get the names of each transition we own
    virtual std::vector<std::string> GetTransitionNames() const = 0;
    // delete all the Transition unique_ptrs by clearing the vector
    virtual void ClearTransitions() = 0;
    // return const & to limit to observation of the state. Need copy ability of
    // History, but let that be the History's responsibility
    virtual std::map<std::string, History> GetHistories() const = 0;

    virtual void SetHistories(const std::map<std::string, History> &h) = 0;
    // getter for model name
    virtual std::string GetModelName() const = 0;
    // getter for log name
    virtual std::string GetLogName() const = 0;

    /// @brief Factory method to create a Markov instance.
    /// @param log_name Name of the logger to write errors to.
    /// @return An instance of Markov.
    static std::unique_ptr<Model>
    Create(const std::string &name, const std::string &log_name = "console");
};
} // namespace respond

#endif // RESPOND_MODEL_HPP_