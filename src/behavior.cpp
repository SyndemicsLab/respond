////////////////////////////////////////////////////////////////////////////////
// File: behavior.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/behavior.hpp"

#include <memory>
#include <sstream>
#include <string>

#include <respond/logging.hpp>
#include <spdlog/spdlog.h>

namespace respond {
Eigen::VectorXd
Behavior::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                  std::map<std::string, History> &h) const {
    TestCorrectNumberMatrices(1);
    TestRowColDimensions(state, GetMatrices()[0]);
    Eigen::VectorXd new_state = GetMatrices()[0] * state;
    return new_state;
}
} // namespace respond