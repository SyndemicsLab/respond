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
        : _name(name), _log_name(log_name), _current_timestep(0),
          _history_capture_interval(1), _final_timestep(-1),
          _initial_history_recorded(false) {
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
        np->SetHistoryCaptureInterval(GetHistoryCaptureInterval());
        np->SetFinalTimestep(GetFinalTimestep());
        if (auto *markov = dynamic_cast<Markov *>(np.get())) {
            markov->_current_timestep = _current_timestep;
            markov->_initial_history_recorded = _initial_history_recorded;
        }
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
        std::map<std::string, History> ret;
        ret["state"] = History("state", GetLogName(), HistoryMode::Snapshot);
        ret["total_overdose"] =
            History("total_overdose", GetLogName(), HistoryMode::Accumulated);
        ret["fatal_overdose"] =
            History("fatal_overdose", GetLogName(), HistoryMode::Accumulated);
        ret["intervention_admission"] = History(
            "intervention_admission", GetLogName(), HistoryMode::Accumulated);
        ret["background_death"] = History("background_death", GetLogName(),
                                             HistoryMode::Accumulated);
        SetHistories(ret);
    }

    // manipulate the state vector
    void RunTransitions() override {
        SetupHistory();
        if (!_initial_history_recorded) {
            RecordHistoryAtCurrentTimestep();
        }
        for (const auto &t : _transition_vector) {
            _state = t->Execute(_state, _histories);
        }
        _current_timestep++;
        RecordHistoryAtCurrentTimestep();
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
        if (_histories.empty()) {
            ResetHistoryTracking();
            return;
        }

        const int latest_timestep = GetLatestRecordedTimestep();
        if (latest_timestep < 0) {
            ResetHistoryTracking();
            return;
        }

        _initial_history_recorded = true;
        _current_timestep = latest_timestep;
    }
    void ClearHistories() override {
        _histories.clear();
        ResetHistoryTracking();
    }

    void SetHistoryCaptureInterval(int interval) override {
        _history_capture_interval = (interval < 1) ? 1 : interval;
    }

    int GetHistoryCaptureInterval() const override {
        return _history_capture_interval;
    }

    void SetFinalTimestep(int final_timestep) override {
        _final_timestep = final_timestep;
    }

    int GetFinalTimestep() const override { return _final_timestep; }

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
    int _current_timestep;
    int _history_capture_interval;
    int _final_timestep;
    bool _initial_history_recorded;

    void ResetHistoryTracking() {
        _current_timestep = 0;
        _initial_history_recorded = false;
    }

    int GetLatestRecordedTimestep() const {
        int latest = -1;
        for (const auto &kv : _histories) {
            latest = std::max(latest, kv.second.GetLatestRecordedTimestep());
        }
        return latest;
    }

    bool ShouldRecordHistoryAtTimestep(int timestep) const {
        if (timestep == 0) {
            return true;
        }
        if (_final_timestep >= 0 && timestep == _final_timestep) {
            return true;
        }
        return timestep % _history_capture_interval == 0;
    }

    void RecordHistoryAtCurrentTimestep() {
        if (_initial_history_recorded && _current_timestep == 0) {
            return;
        }
        if (!ShouldRecordHistoryAtTimestep(_current_timestep)) {
            return;
        }

        _histories["state"].RecordSnapshot(_state, _current_timestep);
        const auto size = _state.size();
        _histories["intervention_admission"].FlushPendingState(
            _current_timestep, size);
        _histories["total_overdose"].FlushPendingState(_current_timestep,
                                                        size);
        _histories["fatal_overdose"].FlushPendingState(_current_timestep,
                                                        size);
        _histories["background_death"].FlushPendingState(_current_timestep,
                                                          size);
        _initial_history_recorded = true;
    }

    void SetupHistory() {
        if (_histories.empty()) {
            CreateDefaultHistories();
        }
    }
};
} // namespace respond

#endif // RESPOND_INTERNALS_MARKOV_HPP_