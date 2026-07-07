////////////////////////////////////////////////////////////////////////////////
// File: background.cpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/logging.hpp>

#include <memory>
#include <string>

#include <Eigen/Dense>

#include "internals/background.hpp"

namespace respond {
Eigen::VectorXd
BackgroundDeath::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                         std::map<std::string, History> &h) const {
    if (GetMatrices().size() != 1) {
        std::string error_msg =
            "Background death error: Expected 1 transition matrix, got " +
            std::to_string(GetMatrices().size());
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    Eigen::VectorXd deaths =
        state.cwiseProduct(GetMatrices()[0]); // calculate the deaths
    if (h.find("background_death") != h.end()) {
        h["background_death"].AccumulateState(deaths);
    }
    if (!(state.array() >= deaths.array()).all()) {
        std::string error_msg =
            "Background death error: State values are less than estimated "
            "deaths. " +
            std::to_string((state.array() < deaths.array()).count()) +
            " elements affected";
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    auto new_state = state - deaths; // remove deaths from state
    return new_state;
}
} // namespace respond