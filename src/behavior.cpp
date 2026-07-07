////////////////////////////////////////////////////////////////////////////////
// File: behavior.cpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/behavior.hpp"

#include <memory>
#include <string>

#include <respond/logging.hpp>
#include <spdlog/spdlog.h>

namespace respond {
Eigen::VectorXd
Behavior::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                  std::map<std::string, History> &h) const {
    if (GetMatrices().size() != 1) {
        std::string error_msg =
            "Behavior error: Expected 1 transition matrix, got " +
            std::to_string(GetMatrices().size());
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    if (state.rows() != GetMatrices()[0].cols()) {
        std::stringstream ss;
        ss << "Behavior error: State dimension mismatch. State size is ("
           << state.rows() << ", " << state.cols()
           << ") but transition matrix expects (" << GetMatrices()[0].rows()
           << ", " << GetMatrices()[0].cols() << ")";
        std::string error_msg = ss.str();
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    auto new_state = GetMatrices()[0] * state;
    return new_state;
}
} // namespace respond