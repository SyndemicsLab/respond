////////////////////////////////////////////////////////////////////////////////
// File: respond.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-01                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_HPP_
#define RESPOND_HPP_

#include <exception>
#include <sstream>
#include <vector>

#include <Eigen/Dense>

#include <respond/cost_effectiveness.hpp>
#include <respond/logging.hpp>
#include <respond/markov.hpp>
#include <respond/types.hpp>

namespace respond {
Eigen::VectorXd Migration(Eigen::VectorXd &state,
                          const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Migration Transitions must have 1 Transition Matrix.");
    }
    if (state.size() != transition[0].size()) {
        throw std::runtime_error("Unable to add Migration Transition Vector to "
                                 "State Vector, mismatched sizes.");
    }
    state += transition[0];
    return state;
}

Eigen::VectorXd Behavior(Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Behavior Transitions must have 1 Transition Matrix.");
    }
    if (state.rows() != transition[0].cols()) {
        std::stringstream ss;
        ss << "Unable to multiply behavior transition with "
              "state, mismatched sizes. State size is (";
        ss << state.rows() << ", " << state.cols();
        ss << ") and transition size is (" << transition[0].rows() << ", ";
        ss << transition[0].cols() << ").";
        throw std::runtime_error(ss.str());
    }
    state = transition[0] * state;
    return state;
}

Eigen::VectorXd Intervention(Eigen::VectorXd &state,
                             const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 2) {
        throw std::runtime_error(
            "Intervention Transitions must have 2 Transition Matrices.");
    }

    Eigen::VectorXd zero_matrix = Eigen::VectorXd::Zero(state.size());

    auto inter = transition[0] * state; // interventions
    auto moved = (inter - state);       // calculate the people that moved

    auto negatives = moved.cwiseMin(zero_matrix);
    auto positives = moved.cwiseMax(zero_matrix);

    auto iie = transition[1] * positives; // transition the people

    if (positives.sum() != iie.sum()) {
        throw std::runtime_error(
            "Intervention Transitions must maintain the same number of people "
            "during the transition.");
    }

    auto total_moved = iie + negatives;

    state += total_moved; // add the people back to the state
    return state;
}

Eigen::VectorXd Overdose(Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 2) {
        throw std::runtime_error(
            "Overdose Transitions must have 2 Transition Matrices.");
    }

    Eigen::VectorXd zero_matrix = Eigen::VectorXd::Zero(state.size());
    Eigen::VectorXd overdoses = state.cwiseProduct(transition[0]); // overdose

    if (overdoses.size() != transition[1].size()) {
        throw std::runtime_error("Fatal Overdose Transition is not the same "
                                 "size as the state vector.");
    }

    auto fods = overdoses.cwiseProduct(transition[1]); // negatives

    Eigen::VectorXd ods_moves = state - fods; // put the fods back
    ods_moves.bottomRows(state.size() / 3) =
        state.bottomRows(state.size() / 3) +
        fods(Eigen::seqN(state.size() / 3, state.size() / 3));

    state = ods_moves; // transition fatal overdoses
    return overdoses;  // return number of total overdoses
}

Eigen::VectorXd Mortality(Eigen::VectorXd &state,
                          const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Mortality Transitions must have 1 Transition Matrix.");
    }
    state = transition[0] * state;
    return state;
}
} // namespace respond

#endif