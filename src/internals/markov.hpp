////////////////////////////////////////////////////////////////////////////////
// File: markov.hpp                                                           //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-09                                                  //
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

#include <respond/constants.hpp>
#include <respond/history.hpp>
#include <respond/transition.hpp>

namespace respond {
class Markov : public virtual Model {
public:
    ////////////////////////////////////////////////////////////////////////////
    //
    // Rule of Five: Copy and Move Semantics
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Default constructor for Markov model. Initializes with default
    /// name "markov" and logger "console".
    Markov() : Markov("markov", RESPOND_DEFAULT_LOG) {}

    /// @brief Constructs a Markov model with specified name and logger.
    /// @param name The identifier for this model.
    /// @param log_name The logger name for error reporting.
    Markov(const std::string &name, const std::string &log_name)
        : Markov(name, log_name, RESPOND_DEFAULT_LOG_FILE) {}

    /// @brief Constructs a Markov model with specified name, logger, and log
    /// file path.
    /// @param name The identifier for this model.
    /// @param log_name The logger name for error reporting.
    /// @param log_filepath The file path for the log file to be used by this
    /// model.
    Markov(const std::string &name, const std::string &log_name,
           const std::string &log_filepath)
        : _name(name), _log_name(log_name), _current_timestep(0),
          _history_capture_interval(1), _final_timestep(-1),
          _initial_history_recorded(false) {
        CreateFileLogger(log_name, log_filepath);
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
    }

    /// @brief Destructor for Markov model. Default implementation.
    ~Markov() = default;

    Markov(Markov &&other) noexcept {
        _state = other._state;
        _name = other._name;
        _log_name = other._log_name;
        _current_timestep = other._current_timestep;
        _history_capture_interval = other._history_capture_interval;
        _final_timestep = other._final_timestep;
        _initial_history_recorded = other._initial_history_recorded;
        for (const auto &h : other._histories) {
            _histories[h.first] = h.second;
        }
        other._histories.clear();
        for (const auto &t : other._timestep_vector) {
            _timestep_vector.push_back(std::move(t));
        }
        other.ClearTimesteps();
    }
    Markov &operator=(Markov &&other) noexcept {
        if (this != &other) {
            _state = other._state;
            _name = other._name;
            _log_name = other._log_name;
            _current_timestep = other._current_timestep;
            _history_capture_interval = other._history_capture_interval;
            _final_timestep = other._final_timestep;
            _initial_history_recorded = other._initial_history_recorded;
            for (const auto &h : other._histories) {
                _histories[h.first] = h.second;
            }
            other._histories.clear();
            for (const auto &t : other._timestep_vector) {
                _timestep_vector.push_back(std::move(t));
            }
            other.ClearTimesteps();
        }
        return *this;
    }

    /// @brief Function to provide a deep copy operation of the Model. Copy
    /// constructor and assignment operator are deleted to prevent copying of
    /// Markov instances. Instead we prefer to use `clone()` for deep copying.
    /// @return A unique_ptr to a new Markov instance that is a deep copy of
    /// this instance.
    std::unique_ptr<Model> clone() const override {
        auto np = Model::Create(_name, _log_name);
        np->SetState(GetState());
        np->SetHistoryCaptureInterval(GetHistoryCaptureInterval());
        np->SetFinalTimestep(GetFinalTimestep());
        if (auto *markov = dynamic_cast<Markov *>(np.get())) {
            markov->_histories = _histories;
            markov->_current_timestep = _current_timestep;
            markov->_initial_history_recorded = _initial_history_recorded;
        }

        int timesteps = static_cast<int>(_timestep_vector.size());
        for (int i = 0; i < timesteps; ++i) {
            np->AddTimestep(_timestep_vector[i]);
        }
        return np;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Getters and Setters for Model State and Metadata
    //
    ////////////////////////////////////////////////////////////////////////////

    Timestep GetTimestepAtIndex(size_t index) const override {
        if (index >= _timestep_vector.size()) {
            throw std::out_of_range("Index out of range in GetTimestepAtIndex");
        }
        return _timestep_vector[index];
    }

    const Eigen::Ref<const Eigen::VectorXd> GetState() const override {
        return _state;
    }

    std::string GetName() const override { return _name; }

    const std::map<std::string, History> &GetHistories() const override {
        return _histories;
    }

    int GetTimestep() const override { return _current_timestep; }

    int GetHistoryCaptureInterval() const override {
        return _history_capture_interval;
    }

    int GetFinalTimestep() const override { return _final_timestep; }

    bool GetInitialHistoryRecorded() const override {
        return _initial_history_recorded;
    }

    void SetState(const Eigen::Ref<const Eigen::VectorXd> &s) override {
        _state = s;
    }

    void SetHistoryCaptureInterval(int interval) override {
        _history_capture_interval = (interval < 1) ? 1 : interval;
    }

    void SetFinalTimestep(int final_timestep) override {
        _final_timestep = final_timestep;
    }

    void SetInitialHistoryRecorded(bool recorded) override {
        _initial_history_recorded = recorded;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Model Behavior Methods: Timestep Execution and History Management
    //
    ////////////////////////////////////////////////////////////////////////////

    void AddTimestep(const Timestep &timestep) override {
        _timestep_vector.push_back(timestep);
        if (static_cast<int>(_timestep_vector.size()) > _final_timestep) {
            LogWarning(_log_name, "Final timestep exceeded by added timestep.");
        }
    }

    void RunTimestep() override {
        RunTimestep(_current_timestep);
        _current_timestep++;
    }

    /// @brief Executes the timestep in the model's sequence.
    void RunTimestep(size_t idx) override {
        if (_timestep_vector.empty()) {
            LogWarning(_log_name,
                       "No timesteps available to run for model: " + _name);
            return;
        }

        if (idx >= static_cast<int>(_timestep_vector.size())) {
            LogWarning(
                _log_name,
                "Current timestep exceeds available timesteps for model: " +
                    _name);
            return;
        }

        auto transitions = _timestep_vector[idx].GetTransitions();
        for (const auto &t : transitions) {
            _state = t->Execute(_state, _histories);
        }
    }

    void RunTimesteps() override {
        SetupHistory();
        if (!_initial_history_recorded) {
            RecordHistoryAtCurrentTimestep();
        }
        for (size_t i = 0; i < _timestep_vector.size(); ++i) {
            RunTimestep();
            RecordHistoryAtCurrentTimestep();
        }
    }

    void ClearTimesteps() override { _timestep_vector.clear(); }

    void ClearHistories() override {
        _histories.clear();
        ResetHistoryTracking();
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
        ret["state"] = History("state", _log_name, HistoryMode::Snapshot);
        ret["total_overdose"] =
            History("total_overdose", _log_name, HistoryMode::Accumulated);
        ret["fatal_overdose"] =
            History("fatal_overdose", _log_name, HistoryMode::Accumulated);
        ret["intervention_admission"] = History(
            "intervention_admission", _log_name, HistoryMode::Accumulated);
        ret["background_death"] =
            History("background_death", _log_name, HistoryMode::Accumulated);
        _histories = ret;
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

private:
    std::vector<Timestep> _timestep_vector;
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
        _histories["total_overdose"].FlushPendingState(_current_timestep, size);
        _histories["fatal_overdose"].FlushPendingState(_current_timestep, size);
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