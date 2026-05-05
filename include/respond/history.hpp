////////////////////////////////////////////////////////////////////////////////
// File: history.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_HISTORY_HPP_
#define RESPOND_HISTORY_HPP_

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include <Eigen/Dense>

namespace respond {
enum class HistoryMode { Snapshot, Accumulated };

inline HistoryMode GetDefaultHistoryMode(const std::string &name) {
    if (name == "intervention_admission" || name == "total_overdose" ||
        name == "fatal_overdose" || name == "background_death") {
        return HistoryMode::Accumulated;
    }
    return HistoryMode::Snapshot;
}

/// @brief Tracks and manages state vector history over time.
/// History records state snapshots at discrete timesteps, enabling analysis of
/// state trajectories during model execution. Supports sparse timesteps (gaps
/// are filled with zero vectors).
class History {
public:
    /// @brief Constructs a History tracker.
    /// @param name The identifier for this history (default: "state").
    /// @param log_name The logger name for error reporting (default:
    /// "console").
    History(const std::string &name = "state",
            const std::string &log_name = "console")
        : History(name, log_name, GetDefaultHistoryMode(name)) {}

    /// @brief Constructs a History tracker with an explicit recording mode.
    /// @param name The identifier for this history.
    /// @param log_name The logger name for error reporting.
    /// @param mode Whether the history stores snapshots or accumulations.
    History(const std::string &name, const std::string &log_name,
            HistoryMode mode)
        : _log_name(log_name), _name(name), _mode(mode) {}

    /// @brief Destructor (default).
    ~History() = default;
    /// @brief Copy constructor implementing the Rule of Five.
    /// Creates an independent copy of the history state and metadata.
    History(const History &other) {
        _timesteps = other.GetRecordedTimesteps();
        _states = other.GetRecordedStates();
        _name = other.GetHistoryName();
        _log_name = other.GetLogName();
        _mode = other.GetHistoryMode();
        _pending_state = other.GetPendingState();
    }

    /// @brief Copy assignment operator implementing the Rule of Five.
    /// @param other The history to copy from.
    /// @return Reference to this history after assignment.
    History &operator=(const History &other) {
        if (this != &other) {
            _timesteps = other.GetRecordedTimesteps();
            _states = other.GetRecordedStates();
            _name = other.GetHistoryName();
            _log_name = other.GetLogName();
            _mode = other.GetHistoryMode();
            _pending_state = other.GetPendingState();
        }
        return *this;
    }

    /// @brief Move constructor implementing the Rule of Five.
    /// @param other The history to move from (leaves original state unchanged
    /// per current implementation).
    History(History &&other) noexcept {
        _timesteps = std::move(other._timesteps);
        _states = std::move(other._states);
        _name = other.GetHistoryName();
        _log_name = other.GetLogName();
        _mode = other.GetHistoryMode();
        _pending_state = std::move(other._pending_state);
    }

    /// @brief Move assignment operator implementing the Rule of Five.
    /// @param other The history to move from.
    /// @return Reference to this history after assignment.
    History &operator=(History &&other) noexcept {
        if (this != &other) {
            _timesteps = std::move(other._timesteps);
            _states = std::move(other._states);
            _name = other.GetHistoryName();
            _log_name = other.GetLogName();
            _mode = other.GetHistoryMode();
            _pending_state = std::move(other._pending_state);
        }
        return *this;
    }

    /// @brief Equality comparison operator.
    /// @param other The history to compare with.
    /// @return True if all history properties and state are identical.
    bool operator==(const History &other) const {
        return GetHistoryName() == other.GetHistoryName() &&
               GetLogName() == other.GetLogName() &&
               GetHistoryMode() == other.GetHistoryMode() &&
               GetStateMap() == other.GetStateMap() &&
               GetPendingState().isApprox(other.GetPendingState());
    }

    /// @brief Inequality comparison operator.
    /// @param other The history to compare with.
    /// @return True if histories differ in any aspect.
    bool operator!=(const History &other) const { return !(*this == other); }

    /// @brief Retrieves the complete state map (timestep -> state vector).
    /// @return Map of integer timesteps to Eigen vectors representing states.
    std::map<int, Eigen::VectorXd> GetStateMap() const {
        std::map<int, Eigen::VectorXd> state_map;
        for (size_t index = 0; index < _timesteps.size(); ++index) {
            state_map[_timesteps[index]] = _states[index];
        }
        return state_map;
    }

    /// @brief Retrieves the recorded timesteps without densifying gaps.
    /// @return Const reference to the stored timestep indices.
    const std::vector<int> &GetRecordedTimesteps() const { return _timesteps; }

    /// @brief Retrieves the recorded state vectors without densifying gaps.
    /// @return Const reference to the stored state vectors.
    const std::vector<Eigen::VectorXd> &GetRecordedStates() const {
        return _states;
    }

    /// @brief Retrieves the configured history recording mode.
    /// @return Snapshot or accumulated history mode.
    HistoryMode GetHistoryMode() const { return _mode; }

    /// @brief Indicates whether an accumulated history has pending state.
    /// @return True when a pending aggregate exists.
    bool HasPendingState() const { return _pending_state.size() > 0; }

    /// @brief Retrieves the pending accumulated state.
    /// @return The pending aggregate vector, or an empty vector if none.
    Eigen::VectorXd GetPendingState() const { return _pending_state; }

    /// @brief Retrieves the latest recorded timestep.
    /// @return Largest recorded timestep, or -1 if history is empty.
    int GetLatestRecordedTimestep() const {
        if (_timesteps.empty()) {
            return -1;
        }
        return _timesteps.back();
    }

    /// @brief Retrieves the identifier name of this history.
    /// @return The history's name string.
    std::string GetHistoryName() const { return _name; }

    /// @brief Retrieves the logger name for this history.
    /// @return The associated logger's name.
    std::string GetLogName() const { return _log_name; }

    /// @brief Converts the sparse history map to a contiguous vector of states.
    /// Gaps in timesteps are filled with zero vectors of appropriate dimension.
    /// @return Vector of Eigen vectors from timestep 0 to the maximum recorded
    /// timestep. Returns empty vector if no state has been recorded.
    std::vector<Eigen::VectorXd> GetStateAsVector() const {
        std::vector<Eigen::VectorXd> ret;
        if (_states.empty()) {
            // warn empty state vector - no states recorded
            return {};
        }
        int default_size = _states.front().size();
        int tstep = 0;
        for (size_t index = 0; index < _timesteps.size(); ++index) {
            const int recorded_timestep = _timesteps[index];
            const auto &recorded_state = _states[index];
            if (recorded_timestep > tstep) {
                // Fill gap: raise error if timestep mapping is invalid
            }
            while (recorded_timestep > tstep) {
                ret.push_back(GetZeroVector(default_size));
                tstep++;
            }
            ret.push_back(recorded_state);
            tstep++;
        }
        return ret;
    }

    /// @brief Records a state vector at a specific or automatic timestep.
    /// @param state The state vector to record.
    /// @param timestep The timestep index for this state (default: -1 for
    /// automatic next timestep). If timestep is negative, the next sequential
    /// timestep is used automatically. If timestep already exists, it is
    /// considered invalid but is currently overwritten.
    void AddState(const Eigen::VectorXd &state, int timestep = -1) {
        if (timestep < 0) {
            timestep = GetNextTimestep();
        }

        const auto existing = std::find(_timesteps.begin(), _timesteps.end(),
                                        timestep);
        if (existing != _timesteps.end()) {
            const auto index =
                static_cast<size_t>(existing - _timesteps.begin());
            _states[index] = state;
            return;
        }

        _timesteps.push_back(timestep);
        _states.push_back(state);
    }

    /// @brief Records a snapshot value at a concrete timestep.
    /// @param state The snapshot value to record.
    /// @param timestep The simulation timestep for this snapshot.
    void RecordSnapshot(const Eigen::VectorXd &state, int timestep) {
        AddState(state, timestep);
    }

    /// @brief Adds a contribution to an accumulated history.
    /// @param state The per-step contribution to accumulate.
    void AccumulateState(const Eigen::VectorXd &state) {
        if (_mode != HistoryMode::Accumulated) {
            AddState(state);
            return;
        }

        if (_pending_state.size() == 0) {
            _pending_state = state;
            return;
        }
        _pending_state += state;
    }

    /// @brief Flushes pending accumulated state into a recorded timestep.
    /// @param timestep The simulation timestep to record.
    /// @param state_size Size of a zero vector to record if nothing is pending.
    void FlushPendingState(int timestep, Eigen::Index state_size) {
        if (_mode != HistoryMode::Accumulated) {
            return;
        }

        Eigen::VectorXd value;
        if (_pending_state.size() > 0) {
            value = _pending_state;
        } else {
            value = Eigen::VectorXd::Zero(state_size);
        }

        AddState(value, timestep);
        _pending_state.resize(0);
    }

    /// @brief Clears all recorded state history.
    void Clear() {
        _timesteps.clear();
        _states.clear();
        _pending_state.resize(0);
    }

private:
    /// @brief The logger name for this history.
    std::string _log_name;
    /// @brief The identifier name for this history.
    std::string _name;
    /// @brief Controls whether this history stores snapshots or aggregates.
    HistoryMode _mode;
    /// @brief Recorded timestep indices for sparse history capture.
    std::vector<int> _timesteps;
    /// @brief Recorded state vectors aligned with _timesteps.
    std::vector<Eigen::VectorXd> _states;
    /// @brief Pending aggregate for accumulated histories.
    Eigen::VectorXd _pending_state;

    /// @brief Computes the next sequential timestep.
    /// @return 0 if history is empty, otherwise one past the largest existing
    /// timestep.
    int GetNextTimestep() {
        if (_timesteps.empty()) {
            return 0;
        }
        return _timesteps.back() + 1;
    }

    /// @brief Creates a zero vector of specified size.
    /// @param size The dimensionality of the zero vector.
    /// @return An Eigen vector of zeros with the specified size.
    Eigen::VectorXd GetZeroVector(const int &size) const {
        return Eigen::VectorXd::Zero(size);
    }
};
} // namespace respond

#endif // RESPOND_HISTORY_HPP_