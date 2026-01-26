////////////////////////////////////////////////////////////////////////////////
// File: respond.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-01-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
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
Eigen::VectorXd Migration(const Eigen::VectorXd &state,
                          const std::vector<Eigen::MatrixXd> &transition,
                          HistoryStamp &stamp) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Migration Transitions must have 1 Transition Matrix.");
    }
    if (state.size() != transition[0].size()) {
        throw std::runtime_error("Unable to add Migration Transition Vector to "
                                 "State Vector, mismatched sizes.");
    }
    auto new_state = state + transition[0];
    return new_state;
}

/// @brief A function to model the relapsing/remitting behavior of SUD.
/// @param state The model state vector.
/// @param transition A vector of size 1 containing the transition matrix for
/// the behavior changes.
/// @return The resultant model state vector.
Eigen::VectorXd Behavior(const Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition,
                         HistoryStamp &stamp) {
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
    auto new_state = transition[0] * state;
    return new_state;
}

/// @brief A function to model the intervention changes of SUD.
/// @param state The model state vector.
/// @param transition A vector of size 1 containing the transition matrix
/// for intervention changes.
/// @return The resultant model state vector.
Eigen::VectorXd Intervention(const Eigen::VectorXd &state,
                             const std::vector<Eigen::MatrixXd> &transition,
                             HistoryStamp &stamp) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Intervention Transitions must have 1 Transition Matrix.");
    }

    Eigen::VectorXd zero_matrix = Eigen::VectorXd::Zero(state.size());
    if (state.rows() != transition[0].cols()) {
        std::stringstream ss;
        ss << "Unable to multiply intervention transition with "
              "state, mismatched sizes. State size is (";
        ss << state.rows() << ", " << state.cols();
        ss << ") and transition size is (" << transition[0].rows() << ", ";
        ss << transition[0].cols() << ").";
        throw std::runtime_error(ss.str());
    }
    auto moved = transition[0] * state;

    // Add intervention_admissions to HistoryStamp
    Eigen::VectorXd admissions = moved - state;
    admissions = admissions.cwiseMax(Eigen::VectorXd::Zero(admissions.size()));
    stamp.intervention_admissions = admissions;

    return moved;
}

/// @brief A function to model the overdoses within the SUD community.
/// @param state The model state vector.
/// @param transition A vector of size 2 containing first the transition matrix
/// probability and second a vector of the chance of an overdose being fatal.
/// @return The number of overdoses that occurred this iteration.
Eigen::VectorXd Overdose(const Eigen::VectorXd &state,
                         const std::vector<Eigen::MatrixXd> &transition,
                         HistoryStamp &stamp) {
    if (transition.size() != 2) {
        throw std::runtime_error(
            "Overdose Transitions must have 2 Transition Matrices.");
    }

    if (state.size() != transition[0].size()) {
        throw std::runtime_error("Overdose Vector is not the same "
                                 "size as the state vector.");
    }
    Eigen::VectorXd overdoses = state.cwiseProduct(transition[0]); // overdose
    // Add total overdoses to stamp
    stamp.total_overdoses = overdoses;

    if (overdoses.size() != transition[1].size()) {
        throw std::runtime_error("Fatal Overdose Vector is not the same "
                                 "size as the state vector.");
    }
    auto fods = overdoses.cwiseProduct(transition[1]); // negatives
    stamp.fatal_overdoses = fods;  // Add fatal overdoses to stamp
    auto new_state = state - fods; // remove fods from state
    return new_state;
}

/// @brief A function to model the mortality within the SUD community.
/// @param state The model state vector.
/// @param transition A vector of size 1 containing the transition matrix for
/// background mortalities.
/// @return The resultant state vector.
Eigen::VectorXd Mortality(const Eigen::VectorXd &state,
                          const std::vector<Eigen::MatrixXd> &transition,
                          HistoryStamp &stamp) {
    if (transition.size() != 1) {
        throw std::runtime_error(
            "Mortality Transitions must have 1 Transition Matrix.");
    }
    auto deaths = transition[0] * state; // calculate the deaths
    stamp.background_mortality = deaths; // store the deaths
    auto new_state = state - deaths;     // remove deaths from state
    return new_state;
}
} // namespace respond

#endif