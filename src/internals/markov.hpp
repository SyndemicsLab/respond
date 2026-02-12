////////////////////////////////////////////////////////////////////////////////
// File: markov.hpp                                                           //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-12                                                  //
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
    Markov() : Markov("markov", "console") {}
    Markov(const std::string &name, const std::string &log_name)
        : _name(name), _log_name(log_name) {
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
    }

    // Rule of Five
    ~Markov() = default;

    // Copy
    std::unique_ptr<Model> clone() const override {
        auto np = Model::Create(GetModelName(), GetLogName());
        np->SetState(GetState());
        np->SetHistories(GetHistories());
        for (const auto &t : GetTransitions()) {
            np->AddTransition(t->clone());
        }
        return np;
    }
    // Move
    Markov(Markov &&other) noexcept {
        _state = other.GetState();
        _name = other.GetModelName();
        _log_name = other.GetLogName();
        for (const auto &t : other.GetTransitions()) {
            AddTransition(std::move(t));
        }
        other.ClearTransitions();
    }
    Markov &operator=(Markov &&other) noexcept {
        if (this != &other) {
            _state = other.GetState();
            _name = other.GetModelName();
            _log_name = other.GetLogName();
            for (const auto &t : other.GetTransitions()) {
                AddTransition(std::move(t));
            }
            other.ClearTransitions();
        }
        return *this;
    }

    // anticipate making a copy of the vector
    void SetState(const Eigen::VectorXd &s) override { _state = s; }
    // return const & to limit to observation of the state
    Eigen::VectorXd GetState() const override { return _state; }
    // return the transitions
    const std::vector<std::unique_ptr<Transition>> &GetTransitions() const {
        return _transition_vector;
    }

    /// @brief The default histories are:
    ///     1. State
    ///     2. Total Overdoses
    ///     3. Fatal Overdoses
    ///     4. Intervention Admissions
    ///     5. Background Mortality
    /// @return A vector of the default history objects.
    void CreateDefaultHistories() override {
        std::vector<std::string> names = {
            "state", "total_overdose", "fatal_overdose",
            "intervention_admission", "background_death"};

        std::map<std::string, History> ret;
        for (const auto &n : names) {
            History h(n, GetLogName());
            ret[n] = h;
        }
        SetHistories(ret);
    }

    // manipulate the state vector
    void RunTransitions() override {
        SetupHistory();
        auto histories = GetHistories();
        for (const auto &t : _transition_vector) {
            SetState(t->Execute(GetState(), histories));
        }
        if (histories.find("state") != histories.end()) {
            histories["state"].AddState(GetState());
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

    void SetupHistory() {
        auto histories = GetHistories();
        if (histories.empty()) {
            CreateDefaultHistories();
            histories = GetHistories();
        }
        histories["state"].AddState(GetState());
        auto size = GetState().size();

        histories["intervention_admission"].AddState(
            Eigen::VectorXd::Zero(size));
        histories["total_overdose"].AddState(Eigen::VectorXd::Zero(size));
        histories["fatal_overdose"].AddState(Eigen::VectorXd::Zero(size));
        histories["background_death"].AddState(Eigen::VectorXd::Zero(size));
    }
};
} // namespace respond

#endif // RESPOND_INTERNALS_MARKOV_HPP_