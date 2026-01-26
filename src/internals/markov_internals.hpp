////////////////////////////////////////////////////////////////////////////////
// File: markov_internals.hpp                                                 //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-01-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_MARKOVINTERNALS_HPP_
#define RESPOND_MARKOVINTERNALS_HPP_

#include <respond/markov.hpp>

#include <map>
#include <string>
#include <thread>

#include <Eigen/Dense>

namespace respond {
class MarkovImpl : public virtual Markov {
public:
    /// @brief
    MarkovImpl(const std::string &log_name = "console")
        : _logger_name(log_name) {
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        ResetTime();
        _state = Eigen::VectorXd::Zero(0);
    }

    ~MarkovImpl() = default;

    std::unique_ptr<Markov> clone() const override {
        return std::make_unique<MarkovImpl>(*this);
    }

    MarkovImpl(const MarkovImpl &other) : MarkovImpl(other.GetLoggerName()) {
        SetTransitions(other.GetTransitions());
        SetState(other.GetState());
    }

    MarkovImpl &operator=(const MarkovImpl &other) = delete;

    void SetState(const Eigen::VectorXd &state_vector) override {
        _state = state_vector;
    }
    Eigen::VectorXd GetState() const override { return _state; }

    void SetTransitions(const std::vector<transition> &tr) override {
        _transitions.clear();
        // This is required for a deep copy of the matrices from the vector
        for (const auto &t : tr) {
            _transitions.push_back(t);
        }
    }
    std::vector<transition> GetTransitions() const override {
        return _transitions;
    }

    void AddTransition(const transition &transition) override {
        _transitions.push_back(transition);
    }

    void Run(const int &num_steps) override;

    HistoryOverTime GetRunResults() const override { return _history; }

    std::string GetLoggerName() const override { return _logger_name; }

private:
    std::string _logger_name;
    Eigen::VectorXd _state;
    std::vector<transition> _transitions = {};
    int _time = 0;
    HistoryOverTime _history = {};

    void ResetTime() { _time = 0; }

    void Step(HistoryStamp &hs);

    void LogDebugPoint(const std::string &message,
                       const Eigen::MatrixXd &matrix) const;

    void ResetHistory() { _history.clear(); }

    void RecordInitialHistoryStamp() {
        ResetHistory();
        HistoryStamp stamp;
        stamp.state = _state;
        stamp.intervention_admissions = Eigen::VectorXd::Zero(_state.size());
        stamp.total_overdoses = Eigen::VectorXd::Zero(_state.size());
        stamp.fatal_overdoses = Eigen::VectorXd::Zero(_state.size());
        _history[0] = stamp;
    }
};
} // namespace respond

#endif // RESPOND_MARKOVINTERNALS_HPP_
