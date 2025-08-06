////////////////////////////////////////////////////////////////////////////////
// File: respond.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-06                                                  //
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

/// @brief A namespace for the respond model functionality.
namespace respond {

/// @brief A function to model a migrating cohort.
/// @param state The model state vector.
/// @param transition A vector of size 1 containing an Eigen::VectorXd that
/// migrates into/out of the state.
/// @return The resultant model state vector.
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

/// @brief A function to model the relapsing/remitting behavior of SUD.
/// @param state The model state vector.
/// @param transition A vector of size 1 containing the transition matrix for
/// the behavior changes.
/// @return The resultant model state vector.
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

/// @brief A function to model the intervention changes of SUD.
/// @param state The model state vector.
/// @param transition A vector of size 2 containing first the transition matrix
/// for intervention changes, and then second the behavior changes once going
/// through an intervention change.
/// @return The resultant model state vector.
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

/// @brief A function to model the overdoses within the SUD community.
/// @param state The model state vector.
/// @param transition A vector of size 2 containing first the transition matrix
/// probability and second a vector of the chance of an overdose being fatal.
/// @return The number of overdoses that occurred this iteration.
Eigen::VectorXd Overdose(Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition) {
    if (transition.size() != 2) {
        throw std::runtime_error(
            "Overdose Transitions must have 2 Transition Matrices.");
    }

    if (state.size() != transition[0].size()) {
        throw std::runtime_error("Overdose Vector is not the same "
                                 "size as the state vector.");
    }

    Eigen::VectorXd overdoses = state.cwiseProduct(transition[0]); // overdose

    if (overdoses.size() != transition[1].size()) {
        throw std::runtime_error("Fatal Overdose Vector is not the same "
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

/// @brief A function to model the mortality within the SUD community.
/// @param state The model state vector.
/// @param transition A vector of size 1 containing the transition matrix for
/// background mortalities.
/// @return The resultant state vector.
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