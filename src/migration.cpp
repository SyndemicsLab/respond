////////////////////////////////////////////////////////////////////////////////
// File: migration.cpp                                                        //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/migration.hpp"

#include <memory>
#include <string>

#include <respond/logging.hpp>
#include <spdlog/spdlog.h>

namespace respond {
Eigen::VectorXd
Migration::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                   std::map<std::string, History> &h) const {
    TestCorrectNumberMatrices(1);
    TestMatrixSizes(state, GetMatrices()[0]);
    Eigen::VectorXd subtracted = state + GetMatrices()[0];
    Eigen::VectorXd zero_stop = subtracted.array().max(
        Eigen::VectorXd::Zero(subtracted.size()).array());
    return zero_stop;
}
} // namespace respond