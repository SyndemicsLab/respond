////////////////////////////////////////////////////////////////////////////////
// File: migration.cpp                                                        //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-07                                                  //
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
    if (GetMatrices().size() != 1) {
        std::string error_msg =
            "Migration error: Expected 1 transition matrix, got " +
            std::to_string(GetMatrices().size());
        LogError(GetLogName(), error_msg);
        throw std::runtime_error(error_msg);
    }
    if (state.size() != GetMatrices()[0].size()) {
        std::string error_msg = "Migration error: State size (" +
                                std::to_string(state.size()) +
                                ") does not match transition matrix size (" +
                                std::to_string(GetMatrices()[0].size()) + ")";
        LogError(GetLogName(), error_msg);
        throw std::runtime_error(error_msg);
    }
    auto subtracted = state + GetMatrices()[0];
    auto zero_stop = subtracted.array().max(
        Eigen::VectorXd::Zero(subtracted.size()).array());
    return zero_stop;
}
} // namespace respond