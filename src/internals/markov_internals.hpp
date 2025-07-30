////////////////////////////////////////////////////////////////////////////////
// File: markov_internals.hpp                                                 //
// Project: internals                                                         //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_MODEL_MARKOVINTERNALS_HPP_
#define RESPOND_MODEL_MARKOVINTERNALS_HPP_

#include <respond/model/markov.hpp>

#include <map>
#include <string>
#include <thread>

#include <Eigen/Dense>

using namespace respond::preprocess;

namespace respond {
namespace model {
class MarkovImpl : public virtual Markov {
public:
    /// @brief
    MarkovImpl(const std::string &log_name = "console")
        : _logger_name(log_name) {
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        ResetTime();
    }

    ~MarkovImpl() = default;

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

private:
    const std::string _logger_name;
    Eigen::VectorXd _state;
    std::vector<transition> _transitions = {};
    std::map<int, stamper> _stamper_functions;
    int _time = 0;
    HistoryOverTime _history = {};

    void ResetTime() { _time = 0; }

    void Step(HistoryStamp &hs);

    void LogDebugPoint(const std::string &message,
                       const Eigen::MatrixXd &matrix) const;

    void ResetHistory() { _history.clear(); }

    void ResetStamperFunctions() { _stamper_functions.clear(); }

    void WriteFirstHistoryStamp() {
        ResetHistory();
        HistoryStamp stamp;
        stamp.state = _state;
        _history[0] = stamp;
    }

    void WriteDefaultStamperFunctions() {
        ResetStamperFunctions();
        // Setup the Stamper Functions
        _stamper_functions[2] = [](HistoryStamp &hs, Eigen::VectorXd state,
                                   Eigen::VectorXd moud_movements) {
            Eigen::VectorXd admissions = state - moud_movements;
            Eigen::VectorXd mat = Eigen::VectorXd::Zero(admissions.size());

            admissions = admissions.cwiseMax(mat);
            hs.intervention_admissions = admissions;
        };

        _stamper_functions[3] = [](HistoryStamp &hs, Eigen::VectorXd state,
                                   Eigen::VectorXd od_movements) {
            hs.overdoses = od_movements;
        };
    }
};
} // namespace model
} // namespace respond

#endif // RESPOND_MODEL_MARKOVINTERNALS_HPP_