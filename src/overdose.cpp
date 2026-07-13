////////////////////////////////////////////////////////////////////////////////
// File: overdose.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
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
    auto matrices = GetMatrices();

    TestMatrixSizes(state, GetMatrices()[0]);
    Eigen::VectorXd overdoses = state.cwiseProduct(GetMatrices()[0]);
    TestLessThanState(state, overdoses,
                      "Overdose transition produced more total overdoses than "
                      "available in state.");
    if (h.find("total_overdose") != h.end()) {
        h["total_overdose"].AccumulateState(overdoses);
    }

    TestMatrixSizes(overdoses, GetMatrices()[1]);
    Eigen::VectorXd fods = overdoses.cwiseProduct(GetMatrices()[1]);
    TestLessThanState(state, fods,
                      "Overdose transition produced more fatal overdoses than "
                      "available in state.");
    auto new_state = state - fods;
    if (h.find("fatal_overdose") != h.end()) {
        h["fatal_overdose"].AccumulateState(fods);
    }
    return new_state;
}
} // namespace respond