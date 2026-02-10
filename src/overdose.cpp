////////////////////////////////////////////////////////////////////////////////
// File: overdose.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/overdose.hpp"

#include <memory>
#include <string>

namespace respond {
Eigen::VectorXd Overdose::Execute(const Eigen::VectorXd &state,
                                  std::map<std::string, History> &h) const {
    if (GetTransitionMatrices().size() != 2) {
        throw std::runtime_error(
            "Overdose Transitions must have 2 Transition Matrices.");
    }

    if (state.size() != GetTransitionMatrices()[0].size()) {
        throw std::runtime_error("Overdose Vector is not the same "
                                 "size as the state vector.");
    }
    Eigen::VectorXd overdoses =
        state.cwiseProduct(GetTransitionMatrices()[0]); // overdose
    // Add total overdoses to stamp
    if (h.find("total_overdose") != h.end()) {
        h["total_overdose"].AddState(overdoses);
    }

    if (overdoses.size() != GetTransitionMatrices()[1].size()) {
        throw std::runtime_error("Fatal Overdose Vector is not the same "
                                 "size as the state vector.");
    }
    auto fods = overdoses.cwiseProduct(GetTransitionMatrices()[1]); // negatives
    if (h.find("fatal_overdose") != h.end()) {
        h["fatal_overdose"].AddState(fods);
    }
    auto new_state = state - fods; // remove fods from state
    return new_state;
}

std::unique_ptr<Transition> Overdose::Create(const std::string &name,
                                             const std::string &log_name) {
    return std::make_unique<Overdose>(name, log_name);
}
} // namespace respond