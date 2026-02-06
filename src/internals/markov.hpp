////////////////////////////////////////////////////////////////////////////////
// File: markov.hpp                                                           //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_MARKOV_HPP_
#define RESPOND_INTERNALS_MARKOV_HPP_

#include <respond/model.hpp>

#include <memory>
#include <thread>
#include <vector>

#include <Eigen/Dense>

#include <respond/history.hpp>
#include <respond/transition.hpp>

namespace respond {
class Markov : public virtual Model {
public:
    Markov(const std::string &name, const std::string &log_name)
        : _name(name), _log_name(log_name) {
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
    }

    // Rule of Five
    ~Markov() = default;

    // Copy
    Markov(const Markov &other) {
        SetState(other.GetState());
        _name = other.GetModelName();
        _log_name = other.GetLogName();
        for (const auto &t : other.GetTransitions()) {
            AddTransition(t->clone());
        }
    }
    Markov &operator=(const Markov &other) {
        if (this != &other) {
            _state = other.GetState();
            _name = other.GetModelName();
            _log_name = other.GetLogName();
            for (const auto &t : other.GetTransitions()) {
                AddTransition(t->clone());
            }
        }
        return *this;
    }

    // Move
    Markov(Markov &&other) noexcept {
        _state = other.GetState();
        _name = other.GetModelName();
        _log_name = other.GetLogName();
        for (const auto &t : other.GetTransitions()) {
            // we copy it specifically because we don't want to let people move
            // the unique pointers out of the original model (i.e. force a const
            // constraint on the GetTransitions function)
            AddTransition(t->clone());
        }
        other.ClearTransitions();
    }
    Markov &operator=(Markov &&other) noexcept {
        if (this != &other) {
            _state = other.GetState();
            _name = other.GetModelName();
            _log_name = other.GetLogName();
            for (const auto &t : other.GetTransitions()) {
                // we copy it specifically because we don't want to let people
                // move the unique pointers out of the original model (i.e.
                // force a const constraint on the GetTransitions function)
                AddTransition(t->clone());
            }
            other.ClearTransitions();
        }
        return *this;
    }

    // anticipate making a copy of the vector
    void SetState(const Eigen::VectorXd &s) override { _state = s; }
    // return const & to limit to observation of the state
    Eigen::VectorXd GetState() const override { return _state; }
    const std::vector<std::unique_ptr<Transition>> &GetTransitions() const {
        return _transition_vector;
    }

    // manipulate the state vector
    void RunTransitions() override {
        auto histories = GetHistories();
        for (const auto &t : _transition_vector) {
            t->Execute(GetState(), histories);
        }
        SetHistories(histories);
    }
    // assume ownership of the Transition
    void AddTransition(const std::unique_ptr<Transition> &t) override {
        _transition_vector.push_back(t->clone());
    }
    // get the names of each transition we own
    std::vector<std::string> GetTransitionNames() const override {
        std::vector<std::string> t_names;
        for (const auto &n : _transition_vector) {
            t_names.push_back(n->GetTransitionName());
        }
        return t_names;
    }
    // delete all the Transition unique_ptrs by clearing the vector
    void ClearTransitions() override { _transition_vector.clear(); }

    virtual void
    SetHistories(const std::map<std::string, History> &h) override {
        _histories = h;
    }
    // return const & to limit to observation of the state. Need copy ability of
    // History, but let that be the History's responsibility
    std::map<std::string, History> GetHistories() const override {
        return _histories;
    }
    // getter for model name
    std::string GetModelName() const override { return _name; }
    std::string GetLogName() const override { return _log_name; }

private:
    std::vector<std::unique_ptr<Transition>> _transition_vector;
    Eigen::VectorXd _state;
    std::string _name;
    std::string _log_name;
    std::map<std::string, History> _histories;
};
} // namespace respond

#endif // RESPOND_INTERNALS_MARKOV_HPP_