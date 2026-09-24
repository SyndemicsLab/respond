////////////////////////////////////////////////////////////////////////////////
// File: markov.hpp                                                           //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-09-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_MARKOV_HPP_
#define RESPOND_INTERNALS_MARKOV_HPP_

#include <respond/model.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <Eigen/Dense>

#include <respond/constants.hpp>
#include <respond/eigen_config.hpp>
#include <respond/history.hpp>
#include <respond/logging.hpp>
#include <respond/transition.hpp>

namespace respond {
class Markov : public virtual Model {
public:
    ////////////////////////////////////////////////////////////////////////////
    //
    // Rule of Five: Copy and Move Semantics
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Default constructor for Markov model. Initializes with default..
    Markov() : Markov("markov", RuntimeConfig{}) {}

    /// @brief Constructs a Markov model with specified name and logger.
    /// @param name The identifier for this model.
    /// @param log_name The logger name for error reporting.
    [[deprecated("Use Markov(name, RuntimeConfig) instead")]]
    Markov(const std::string &name, const std::string &log_name)
        : Markov(name, log_name, RESPOND_DEFAULT_LOG_FILE) {}

    /// @brief Constructs a Markov model with specified name, logger, and log
    /// file path.
    /// @param name The identifier for this model.
    /// @param log_name The logger name for error reporting.
    /// @param log_filepath The file path for the log file to be used by this
    /// model.
    /// @param processor_count The number of threads to use when running this
    /// model.
    [[deprecated("Use Markov(name, RuntimeConfig) instead")]]
    Markov(const std::string &name, const std::string &log_name,
           const std::string &log_filepath,
           const unsigned int processor_count =
               std::thread::hardware_concurrency())
        : Markov(name, log_name, log_filepath,
                 ExecutionConfig{0, processor_count, false}) {}

    Markov(const std::string &name, const RuntimeConfig &runtime_config)
        : _name(name), _log_name(runtime_config.logging.logger_name),
          _runtime_config(runtime_config), _current_timestep(0),
          _history_capture_interval(1), _final_timestep(-1),
          _initial_history_recorded(false) {
                if (ConfigureLogger(_runtime_config.logging) == CreationStatus::kError) {
                        throw std::runtime_error(
                                "Error attempting to initialize model logger.");
                }
        const unsigned int thread_limit = std::thread::hardware_concurrency();
        const unsigned int threads =
            thread_limit == 0
                ? _runtime_config.execution.eigen_threads
                : std::min(_runtime_config.execution.eigen_threads,
                           thread_limit);
        detail::SetEigenThreads(threads);
    }

    /// @brief Constructs a Markov model with explicit execution settings.
    /// @param name The identifier for this model.
    /// @param log_name The logger name for error reporting.
    /// @param log_filepath The file path for the log file used by this model.
    /// @param execution_config Resource settings for model execution.
    [[deprecated("Use Markov(name, RuntimeConfig) instead")]]
    Markov(const std::string &name, const std::string &log_name,
           const std::string &log_filepath,
           const ExecutionConfig &execution_config)
        : Markov(name,
                 RuntimeConfig{execution_config,
                               LoggingConfig{log_name, log_filepath, false}}) {}

    /// @brief Destructor for Markov model. Default implementation.
    ~Markov() = default;

    Markov(Markov &&other) noexcept
        : _timestep_vector(std::move(other._timestep_vector)),
          _state(std::move(other._state)), _name(std::move(other._name)),
          _log_name(std::move(other._log_name)),
          _runtime_config(std::move(other._runtime_config)),
          _histories(std::move(other._histories)),
          _current_timestep(other._current_timestep),
          _history_capture_interval(other._history_capture_interval),
          _final_timestep(other._final_timestep),
          _initial_history_recorded(other._initial_history_recorded) {}
    Markov &operator=(Markov &&other) noexcept {
        if (this != &other) {
            _state = std::move(other._state);
            _name = std::move(other._name);
            _log_name = std::move(other._log_name);
            _runtime_config = std::move(other._runtime_config);
            _timestep_vector = std::move(other._timestep_vector);
            _histories = std::move(other._histories);
            _current_timestep = other._current_timestep;
            _history_capture_interval = other._history_capture_interval;
            _final_timestep = other._final_timestep;
            _initial_history_recorded = other._initial_history_recorded;
        }
        return *this;
    }

    /// @brief Function to provide a deep copy operation of the Model. Copy
    /// constructor and assignment operator are deleted to prevent copying of
    /// Markov instances. Instead we prefer to use `clone()` for deep copying.
    /// @return A unique_ptr to a new Markov instance that is a deep copy of
    /// this instance.
    std::unique_ptr<Model> clone() const override {
        auto np = Model::Create(_name, _runtime_config);
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
    }

    void RunTimestep() override { RunTimestep(_current_timestep); }

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

        _current_timestep = static_cast<int>(idx);

        auto transitions = _timestep_vector[idx].GetTransitions();
        for (const auto &t : transitions) {
            _state = t->Execute(_state, _histories);
        }
        _current_timestep++;
    }

    void RunTimesteps() override {
        SetupHistory();
        if (!_initial_history_recorded) {
            RecordHistoryAtCurrentTimestep();
        }
        size_t duration = _timestep_vector.size();
        if (duration > static_cast<size_t>(_final_timestep) &&
            _final_timestep >= 0) {
            std::string warning_msg =
                "Duration is less than available timesteps for model: " +
                _name + ".\nOnly running timesteps up to duration value.";
            LogWarning(_log_name, warning_msg);
            duration = static_cast<size_t>(_final_timestep);
        }

        for (size_t i = 0; i < duration; ++i) {
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
        ret["state"] = History("state", HistoryMode::kSnapshot, _log_name);
        ret["total_overdose"] =
            History("total_overdose", HistoryMode::kAccumulated, _log_name);
        ret["fatal_overdose"] =
            History("fatal_overdose", HistoryMode::kAccumulated, _log_name);
        ret["intervention_admission"] = History(
            "intervention_admission", HistoryMode::kAccumulated, _log_name);
        ret["background_death"] =
            History("background_death", HistoryMode::kAccumulated, _log_name);
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

    void Serialize(std::ostream &os) const override {
        os << "Model Name: " << _name << "\n";
        os << "Current Timestep: " << _current_timestep << "\n";
        os << "History Capture Interval: " << _history_capture_interval << "\n";
        os << "Final Timestep: " << _final_timestep << "\n";
        os << "Initial History Recorded: "
           << (_initial_history_recorded ? "true" : "false") << "\n";
        os << "State Vector: " << _state.transpose() << "\n";
        os << "Histories:\n";
        for (const auto &kv : _histories) {
            os << "  - " << kv.first << "\n";
        }
        os << "Timesteps:\n";
        for (size_t i = 0; i < _timestep_vector.size(); ++i) {
            os << "  Timestep Index: " << i << "\n";
            os << "  Number of Transitions: "
               << _timestep_vector[i].GetTransitions().size() << "\n";
        }
    }

private:
    std::vector<Timestep> _timestep_vector;
    Eigen::VectorXd _state;
    std::string _name;
    std::string _log_name;
    RuntimeConfig _runtime_config;
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

        _histories["state"].AddState(_state, _current_timestep);
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
