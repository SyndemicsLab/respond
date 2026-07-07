////////////////////////////////////////////////////////////////////////////////
// File: intervention.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/intervention.hpp"

#include <memory>
#include <string>

#include <respond/logging.hpp>
#include <spdlog/spdlog.h>

namespace respond {
Eigen::VectorXd
Intervention::Execute(const Eigen::Ref<const Eigen::VectorXd> &state,
                      std::map<std::string, History> &h) const {
    if (GetMatrices().size() != 1) {
        std::string error_msg =
            "Intervention error: Expected 1 transition matrix, got " +
            std::to_string(GetMatrices().size());
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }

    Eigen::VectorXd zero_matrix = Eigen::VectorXd::Zero(state.size());
    if (state.rows() != GetMatrices()[0].cols()) {
        std::stringstream ss;
        ss << "Intervention error: State dimension mismatch. State size is ("
           << state.rows() << ", " << state.cols()
           << ") but transition matrix expects (" << GetMatrices()[0].rows()
           << ", " << GetMatrices()[0].cols() << ")";
        std::string error_msg = ss.str();
        LogError(_log_name, error_msg);
        throw std::runtime_error(error_msg);
    }
    auto moved = GetMatrices()[0] * state;

    // Add intervention_admissions to history if avaliable
    Eigen::VectorXd admissions = moved - state;
    admissions = admissions.cwiseMax(Eigen::VectorXd::Zero(admissions.size()));
    if (h.find("intervention_admission") != h.end()) {
        h["intervention_admission"].AccumulateState(admissions);
    }

    return moved;
}
} // namespace respond