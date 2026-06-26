////////////////////////////////////////////////////////////////////////////////
// File: background.cpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/background.hpp"

#include <memory>
#include <string>

#include <respond/logging.hpp>
#include <spdlog/spdlog.h>

namespace respond {
Eigen::VectorXd
BackgroundDeath::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                         std::map<std::string, History> &h) const {
    if (GetTransitionMatrices().size() != 1) {
        std::string error_msg =
            "Background death error: Expected 1 transition matrix, got " +
            std::to_string(GetTransitionMatrices().size());
        LogError(GetLogName(), error_msg);
        throw std::runtime_error(error_msg);
    }
    auto deaths =
        state.cwiseProduct(GetTransitionMatrices()[0]); // calculate the deaths
    if (h.find("background_death") != h.end()) {
        h["background_death"].AccumulateState(deaths);
    }
    if (!(state.array() >= deaths.array()).all()) {
        std::string error_msg =
            "Background death error: State values are less than estimated "
            "deaths. " +
            std::to_string((state.array() < deaths.array()).count()) +
            " elements affected";
        LogError(GetLogName(), error_msg);
        throw std::runtime_error(error_msg);
    }
    auto new_state = state - deaths; // remove deaths from state
    return new_state;
}

std::unique_ptr<Transition>
BackgroundDeath::Create(const std::string &name, const std::string &log_name) {
    return std::make_unique<BackgroundDeath>(name, log_name);
}
} // namespace respond