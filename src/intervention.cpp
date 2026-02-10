////////////////////////////////////////////////////////////////////////////////
// File: intervention.cpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/intervention.hpp"

#include <memory>
#include <string>

namespace respond {
Eigen::VectorXd Intervention::Execute(const Eigen::VectorXd &state,
                                      std::map<std::string, History> &h) const {
    if (GetTransitionMatrices().size() != 1) {
        throw std::runtime_error(
            "Intervention Transitions must have 1 Transition Matrix.");
    }

    Eigen::VectorXd zero_matrix = Eigen::VectorXd::Zero(state.size());
    if (state.rows() != GetTransitionMatrices()[0].cols()) {
        std::stringstream ss;
        ss << "Unable to multiply intervention transition with "
              "state, mismatched sizes. State size is (";
        ss << state.rows() << ", " << state.cols();
        ss << ") and transition size is (" << GetTransitionMatrices()[0].rows()
           << ", ";
        ss << GetTransitionMatrices()[0].cols() << ").";
        throw std::runtime_error(ss.str());
    }
    auto moved = GetTransitionMatrices()[0] * state;

    // Add intervention_admissions to history if avaliable
    Eigen::VectorXd admissions = moved - state;
    admissions = admissions.cwiseMax(Eigen::VectorXd::Zero(admissions.size()));
    if (h.find("intervention_admission") != h.end()) {
        h["intervention_admission"].AddState(admissions);
    }

    return moved;
}

std::unique_ptr<Transition> Intervention::Create(const std::string &name,
                                                 const std::string &log_name) {
    return std::make_unique<Intervention>(name, log_name);
}
} // namespace respond