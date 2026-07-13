////////////////////////////////////////////////////////////////////////////////
// File: intervention.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
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

    TestCorrectNumberMatrices(1);
    auto trans_matrix = GetMatrices()[0];
    TestSquareMatrix(trans_matrix);
    Eigen::VectorXd zero_matrix = Eigen::VectorXd::Zero(state.size());
    TestRowColDimensions(state, trans_matrix);
    Eigen::VectorXd moved = trans_matrix * state;

    // Add intervention_admissions to history if avaliable
    Eigen::VectorXd admissions = moved - state;
    admissions = admissions.cwiseMax(Eigen::VectorXd::Zero(admissions.size()));
    if (h.find("intervention_admission") != h.end()) {
        h["intervention_admission"].AccumulateState(admissions);
    }

    return moved;
}
} // namespace respond