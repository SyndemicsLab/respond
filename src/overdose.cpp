////////////////////////////////////////////////////////////////////////////////
// File: overdose.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
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
    TestCorrectNumberMatrices(2);

    TestMatrixSizes(state, GetMatrices()[0]);
    Eigen::VectorXd overdoses = state.cwiseProduct(GetMatrices()[0]);
    if (h.find("total_overdose") != h.end()) {
        h["total_overdose"].AccumulateState(overdoses);
    }

    TestMatrixSizes(overdoses, GetMatrices()[1]);
    auto fods = overdoses.cwiseProduct(GetMatrices()[1]); // negatives
    if (h.find("fatal_overdose") != h.end()) {
        h["fatal_overdose"].AccumulateState(fods);
    }
    TestLessThanState(state, fods);
    auto new_state = state - fods; // remove fods from state
    return new_state;
}
} // namespace respond