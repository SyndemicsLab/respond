////////////////////////////////////////////////////////////////////////////////
// File: behavior.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/behavior.hpp"

#include <memory>
#include <string>

namespace respond {
Eigen::VectorXd Behavior::Execute(const Eigen::VectorXd &state,
                                  std::map<std::string, History> &h) const {
    if (GetTransitionMatrices().size() != 1) {
        throw std::runtime_error(
            "Behavior Transitions must have 1 Transition Matrix.");
    }
    if (state.rows() != GetTransitionMatrices()[0].cols()) {
        std::stringstream ss;
        ss << "Unable to multiply behavior transition with "
              "state, mismatched sizes. State size is (";
        ss << state.rows() << ", " << state.cols();
        ss << ") and transition size is (" << GetTransitionMatrices()[0].rows()
           << ", ";
        ss << GetTransitionMatrices()[0].cols() << ").";
        throw std::runtime_error(ss.str());
    }
    auto new_state = GetTransitionMatrices()[0] * state;
    return new_state;
}

std::unique_ptr<Transition> Behavior::Create(const std::string &name,
                                             const std::string &log_name) {
    return std::make_unique<Behavior>(name, log_name);
}
} // namespace respond