////////////////////////////////////////////////////////////////////////////////
// File: migration.cpp                                                        //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/migration.hpp"

#include <memory>
#include <string>

namespace respond {
Eigen::VectorXd Migration::Execute(const Eigen::VectorXd &state,
                                   std::map<std::string, History> &h) const {
    if (GetTransitionMatrices().size() != 1) {
        throw std::runtime_error(
            "Migration Transitions must have 1 Transition Matrix.");
    }
    if (state.size() != GetTransitionMatrices()[0].size()) {
        throw std::runtime_error("Unable to add Migration Transition Vector to "
                                 "State Vector, mismatched sizes.");
    }
    auto new_state = state + GetTransitionMatrices()[0];
    return new_state;
}

std::unique_ptr<Transition> Migration::Create(const std::string &name,
                                              const std::string &log_name) {
    return std::make_unique<Migration>(name, log_name);
}
} // namespace respond