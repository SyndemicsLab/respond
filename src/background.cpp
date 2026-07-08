////////////////////////////////////////////////////////////////////////////////
// File: background.cpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
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
    TestCorrectNumberMatrices(1);
    Eigen::VectorXd deaths = state.cwiseProduct(GetMatrices()[0]);
    TestLessThanState(state, deaths);
    if (h.find("background_death") != h.end()) {
        h["background_death"].AccumulateState(deaths);
    }
    auto new_state = state - deaths; // remove deaths from state
    return new_state;
}
} // namespace respond