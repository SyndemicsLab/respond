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

#include <map>
#include <utility>

#include <Eigen/Dense>

namespace respond {
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
        : _name(name), _log_name(log_name) {}

    /// @brief Destructor (default).
    ~History() = default;
    /// @brief Copy constructor implementing the Rule of Five.
    /// Creates an independent copy of the history state and metadata.
    History(const History &other) {
        _state = other.GetStateMap();
        _name = other.GetHistoryName();
        _log_name = other.GetLogName();
    }

    /// @brief Copy assignment operator implementing the Rule of Five.
    /// @param other The history to copy from.
    /// @return Reference to this history after assignment.
    History &operator=(const History &other) {
        if (this != &other) {
            _state = other.GetStateMap();
            _name = other.GetHistoryName();
            _log_name = other.GetLogName();
        }
        return *this;
    }

    /// @brief Move constructor implementing the Rule of Five.
    /// @param other The history to move from (leaves original state unchanged
    /// per current implementation).
    History(History &&other) noexcept {
        _state = other.GetStateMap();
        _name = other.GetHistoryName();
        _log_name = other.GetLogName();
    }

    /// @brief Move assignment operator implementing the Rule of Five.
    /// @param other The history to move from.
    /// @return Reference to this history after assignment.
    History &operator=(History &&other) noexcept {
        if (this != &other) {
            _state = other.GetStateMap();
            _name = other.GetHistoryName();
            _log_name = other.GetLogName();
        }
        return *this;
    }

    /// @brief Equality comparison operator.
    /// @param other The history to compare with.
    /// @return True if all history properties and state are identical.
    bool operator==(const History &other) const {
        return GetHistoryName() == other.GetHistoryName() &&
               GetLogName() == other.GetLogName() &&
               GetStateMap() == other.GetStateMap();
    }

    /// @brief Inequality comparison operator.
    /// @param other The history to compare with.
    /// @return True if histories differ in any aspect.
    bool operator!=(const History &other) const { return !(*this == other); }

    /// @brief Retrieves the complete state map (timestep -> state vector).
    /// @return Map of integer timesteps to Eigen vectors representing states.
    std::map<int, Eigen::VectorXd> GetStateMap() const { return _state; }

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
        if (_state.empty()) {
            // warn empty state vector - no states recorded
            return {};
        }
        int default_size = _state.begin()->second.size();
        int tstep = 0;
        for (const auto &kv : _state) {
            if (kv.first > tstep) {
                // Fill gap: raise error if timestep mapping is invalid
            }
            while (kv.first > tstep) {
                ret.push_back(GetZeroVector(default_size));
                tstep++;
            }
            ret.push_back(kv.second);
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
        } else if (_state.find(timestep) != _state.end()) {
            // invalid timestep (already exists) - currently overwrites
        }
        _state[timestep] = state;
    }

    /// @brief Clears all recorded state history.
    void Clear() { _state.clear(); }

private:
    /// @brief The logger name for this history.
    std::string _log_name;
    /// @brief The identifier name for this history.
    std::string _name;
    /// @brief Map from timestep index to state vectors.
    std::map<int, Eigen::VectorXd> _state;

    /// @brief Computes the next sequential timestep.
    /// @return 0 if history is empty, otherwise one past the largest existing
    /// timestep.
    int GetNextTimestep() {
        if (_state.empty()) {
            return 0;
        }
        int largest_timestep = _state.rbegin()->first;
        return largest_timestep + 1;
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