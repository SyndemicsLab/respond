////////////////////////////////////////////////////////////////////////////////
// File: markov.cpp                                                           //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-01-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "internals/markov_internals.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <Eigen/Eigen>

#include <respond/logging.hpp>
#include <respond/types.hpp>

namespace respond {

void MarkovImpl::Run(const int &num_steps) {
    ResetTime();

    // All histories now start with t = 0
    RecordInitialHistoryStamp();

    // WriteDefaultStamperFunctions();

    while (_time < num_steps) {
        HistoryStamp stamp;
#ifndef NDEBUG
        respond::LogDebug(_logger_name, "Started Timestep: " + _time);
#endif
        Step(stamp);
        // Writing the history after the timestep is complete
        stamp.state = _state;
        _history[_time + 1] = stamp;
#ifndef NDEBUG
        respond::LogDebug(_logger_name, " | Completed Timestep: " +
                                            std::to_string(_time) + "\n");
#endif
        ++_time;
    }
}

void MarkovImpl::LogDebugPoint(const std::string &message,
                               const Eigen::MatrixXd &matrix) const {
    respond::LogDebug(_logger_name,
                      "-------------------------------------------");
    respond::LogDebug(_logger_name, "Timestep: " + _time);
    std::stringstream ss;
    ss << message << ": " << std::endl << matrix;
    respond::LogDebug(_logger_name, ss.str());
    respond::LogDebug(_logger_name,
                      "-------------------------------------------");
}

void MarkovImpl::Step(HistoryStamp &hs) {
    for (int i = 0; i < _transitions.size(); ++i) {
        transition t = _transitions[i];
        _state = (t.first)(_state, t.second, hs);
    }
}

std::unique_ptr<Markov> Markov::Create(const std::string &log_name) {
    return std::make_unique<MarkovImpl>(log_name);
}
} // namespace respond
