////////////////////////////////////////////////////////////////////////////////
// File: markov.cpp                                                           //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
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
    WriteFirstHistoryStamp();

    WriteDefaultStamperFunctions();

    while (_time < num_steps) {
        HistoryStamp stamp;
        Step(stamp);
        // Writing the history after the timestep is complete
        stamp.state = _state;
        _history[_time + 1] = stamp;
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
        auto temp = (t.first)(_state, t.second);
        auto it = _stamper_functions.find(i);
        if (it != _stamper_functions.end()) {
            it->second(hs, _state, temp);
        }
        _state = temp;
    }
}

std::unique_ptr<Markov> Markov::Create(const std::string &log_name) {
    return std::make_unique<MarkovImpl>(log_name);
}
} // namespace respond
