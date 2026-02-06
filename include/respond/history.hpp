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
class History {
public:
    History(const std::string &name = "state",
            const std::string &log_name = "console")
        : _name(name), _log_name(log_name) {}

    // Rule of Five
    ~History() = default;
    History(const History &other) {
        _state = other.GetStateMap();
        _name = other.GetHistoryName();
        _log_name = other.GetLogName();
    }
    History &operator=(const History &other) {
        if (this != &other) {
            _state = other.GetStateMap();
            _name = other.GetHistoryName();
            _log_name = other.GetLogName();
        }
        return *this;
    }
    History(History &&other) noexcept {
        _state = other.GetStateMap();
        _name = other.GetHistoryName();
        _log_name = other.GetLogName();
    }
    History &operator=(History &&other) noexcept {
        if (this != &other) {
            _state = other.GetStateMap();
            _name = other.GetHistoryName();
            _log_name = other.GetLogName();
        }
        return *this;
    }

    // Getters
    std::map<int, Eigen::VectorXd> GetStateMap() const { return _state; }
    std::string GetHistoryName() const { return _name; }
    std::string GetLogName() const { return _log_name; }

    std::vector<Eigen::VectorXd> GetStateAsVector() const {
        std::vector<Eigen::VectorXd> ret;
        if (_state.empty()) {
            // warn empty state vector
            return {};
        }
        int default_size = _state.begin()->second.size();
        int tstep = 0;
        for (const auto &kv : _state) {
            if (kv.first > tstep) {
                // raise error for invalid key/tstep
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

    void AddState(const Eigen::VectorXd &state, int timestep = -1) {
        if (timestep < 0) {
            timestep = GetNextTimestep();
        } else if (_state.find(timestep) != _state.end()) {
            // invalid timestep (already exists)
        }
        _state[timestep] = state;
    }

    void Clear() { _state.clear(); }

private:
    std::string _log_name;
    std::string _name;
    std::map<int, Eigen::VectorXd> _state;

    int GetNextTimestep() {
        int largest_timestep = _state.rbegin()->first;
        return largest_timestep + 1;
    }

    Eigen::VectorXd GetZeroVector(const int &size) const {
        return Eigen::VectorXd::Zero(size);
    }
};
} // namespace respond

#endif // RESPOND_HISTORY_HPP_