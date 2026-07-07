////////////////////////////////////////////////////////////////////////////////
// File: overdose.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/overdose.hpp"

#include <memory>
#include <string>

#include <respond/logging.hpp>
#include <spdlog/spdlog.h>

namespace respond {
Eigen::VectorXd
Overdose::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                  std::map<std::string, History> &h) const {
    if (GetMatrices().size() != 2) {
        std::string error_msg =
            "Overdose error: Expected 2 transition matrices, got " +
            std::to_string(GetMatrices().size());
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }

    if (state.size() != GetMatrices()[0].size()) {
        std::string error_msg = "Overdose error: State size (" +
                                std::to_string(state.size()) +
                                ") does not match transition matrix size (" +
                                std::to_string(GetMatrices()[0].size()) + ")";
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    Eigen::VectorXd overdoses =
        state.cwiseProduct(GetMatrices()[0]); // overdose
    // Add total overdoses to stamp
    if (h.find("total_overdose") != h.end()) {
        h["total_overdose"].AccumulateState(overdoses);
    }

    if (overdoses.size() != GetMatrices()[1].size()) {
        std::string error_msg = "Overdose error: Fatal overdose vector size (" +
                                std::to_string(overdoses.size()) +
                                ") does not match transition matrix size (" +
                                std::to_string(GetMatrices()[1].size()) + ")";
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    auto fods = overdoses.cwiseProduct(GetMatrices()[1]); // negatives
    if (h.find("fatal_overdose") != h.end()) {
        h["fatal_overdose"].AccumulateState(fods);
    }
    if (!(state.array() >= fods.array()).all()) {
        std::string error_msg =
            "Overdose error: State values are less than estimated fatal "
            "overdoses. " +
            std::to_string((state.array() < fods.array()).count()) +
            " elements affected";
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    auto new_state = state - fods; // remove fods from state
    return new_state;
}
} // namespace respond