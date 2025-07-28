////////////////////////////////////////////////////////////////////////////////
// File: respond.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_HPP_
#define RESPOND_HPP_

#include <exception>
#include <vector>

#include <Eigen/Dense>

#include "model/markov.hpp"
#include "postprocess/cost_effectiveness.hpp"
#include "postprocess/writer.hpp"
#include "preprocess/cost_loader.hpp"
#include "preprocess/data_formatter.hpp"
#include "preprocess/data_loader.hpp"
#include "preprocess/utility_loader.hpp"
#include "utils/logging.hpp"
#include "utils/types.hpp"

namespace respond {
Eigen::VectorXd Migration(Eigen::VectorXd &state,
                          const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Migration Transitions must have 1 Transition Matrix.");
    }
    state *= transition[0];
    return state;
}

Eigen::VectorXd Behavior(Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Behavior Transitions must have 1 Transition Matrix.");
    }
    state *= transition[0];
    return state;
}

Eigen::VectorXd Intervention(Eigen::VectorXd &state,
                             const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 2) {
        throw std::runtime_error(
            "Intervention Transitions must have 2 Transition Matrices.");
    }
    auto inter = state * transition[0]; // interventions
    auto moved = (inter - state);       // calculate the people that moved
    auto iie = moved * transition[1];   // transition the people that moved

    if (moved.sum() != iie.sum()) {
        throw std::runtime_error(
            "Intervention Transitions must maintain the same number of people "
            "during the transition.");
    }

    state = state - moved + iie; // add the people back to the state
    return state;
}

Eigen::VectorXd Overdose(Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 2) {
        throw std::runtime_error(
            "Overdose Transitions must have 2 Transition Matrices.");
    }
    auto overdoses = state * transition[0]; // overdose
    state = overdoses * transition[1];      // transition fatal overdoses
    return overdoses;                       // return number of total overdoses
}

Eigen::VectorXd Mortality(Eigen::VectorXd &state,
                          const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Mortality Transitions must have 1 Transition Matrix.");
    }
    state *= transition[0];
    return state;
}
} // namespace respond

#endif