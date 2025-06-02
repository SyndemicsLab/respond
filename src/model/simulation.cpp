////////////////////////////////////////////////////////////////////////////////
// File: Simulation.cpp                                                       //
// Project: model                                                             //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/simulation_internals.hpp"

#include <Eigen/Eigen>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>
#include <respond/utils/logging.hpp>

using namespace respond::data_ops;

namespace respond {
namespace model {

void RespondImpl::Run(const DataLoader &data_loader) {
    state = data_loader.GetInitialSample();
    ResetTime();
    ResetHistory(state);

    int duration = std::get<int>(data_loader.GetConfig()->get(
        "simulation.duration", static_cast<int>(0)));

    for (; time < duration; ++time) {
        history.state_history[time] = state;
        state = Step(data_loader);
    }
    history.state_history[duration] = state;
}

void RespondImpl::ResetHistory(const Matrix3d &matrix) {
    history.overdose_history.clear();
    history.fatal_overdose_history.clear();
    history.mortality_history.clear();
    history.intervention_admission_history.clear();
    history.state_history.clear();

    // Pad the first timesteps with zeros except for the initial state
    Matrix3d zeros(state.dimensions());
    zeros.setZero();
    history.overdose_history[0] = zeros;
    history.fatal_overdose_history[0] = zeros;
    history.mortality_history[0] = zeros;
    history.intervention_admission_history[0] = zeros;
}

void RespondImpl::LogDebugPoint(const std::string &message,
                                const Matrix3d &matrix) const {
    respond::utils::LogDebug(logger_name,
                             "-------------------------------------------");
    respond::utils::LogDebug(logger_name, "Timestep: " + time);
    std::stringstream ss;
    ss << message << ": " << std::endl << matrix;
    respond::utils::LogDebug(logger_name, ss.str());
    respond::utils::LogDebug(logger_name,
                             "-------------------------------------------");
}

Matrix3d RespondImpl::Step(const DataLoader &data_loader) {
    LogDebugPoint("Starting Simulation Step", state);

    auto matrix_1 = AddEnteringCohort(state, data_loader);

    auto matrix_2 = MultiplyBehaviorTransition(matrix_1, data_loader);

    matrix_1 = MultiplyInterventionTransition(matrix_2, data_loader);

    Matrix3d admissions = state - matrix_1;
    Matrix3d mat(admissions.dimensions());
    mat.setZero();

    admissions = admissions.cwiseMax(mat);
    history.intervention_admission_history[time + 1] = admissions;

    matrix_2 = MultiplyOD(matrix_1, data_loader);
    history.overdose_history[time + 1] = matrix_2;

    auto matrix_3 = MultiplyFODGivenOD(matrix_2, data_loader);
    history.fatal_overdose_history[time + 1] = matrix_3;

    matrix_2 = MultiplyMortality(matrix_1 - matrix_3, data_loader);
    history.mortality_history[time + 1] = matrix_2;

    auto new_state = (matrix_1 - (matrix_2 + matrix_3));

    LogDebugPoint("End of Step", new_state);
    return new_state;
}

Matrix3d RespondImpl::AddEnteringCohort(const Matrix3d &mat,
                                        const DataLoader &data_loader) const {
    auto es = data_loader.GetEnteringSamples(time);
    auto ret = state + es;
    Matrix3d roundingMatrix(es.dimensions());
    roundingMatrix.setZero();
    return ret.cwiseMax(roundingMatrix);
}

Matrix3d
RespondImpl::MultiplyBehaviorTransition(const Matrix3d &state,
                                        const DataLoader &data_loader) const {
    auto behaviorTransitionProbs = data_loader.GetOUDTransitionRates();

    Matrix3d ret = CreateMatrix3d(state.dimension(0), state.dimension(1),
                                  state.dimension(2));

    for (int i = 0; i < state.dimension(static_cast<int>(Dimension::kOud));
         ++i) {

        Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
        auto extentTrans = state.dimensions();
        offsetTrans[static_cast<int>(Dimension::kOud)] =
            i * state.dimension(static_cast<int>(Dimension::kOud));

        Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
        auto extentState = state.dimensions();
        offsetState[static_cast<int>(Dimension::kOud)] = i;
        extentState[static_cast<int>(Dimension::kOud)] = 1;

        auto slicedState = state.slice(offsetState, extentState);

        Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
        bcast[static_cast<int>(Dimension::kOud)] =
            state.dimension(static_cast<int>(Dimension::kOud));

        Matrix3d broadcastedTensor = slicedState.broadcast(bcast);
        Matrix3d slicedTransition =
            behaviorTransitionProbs.slice(offsetTrans, extentTrans);

        ret += (broadcastedTensor * slicedTransition);
    }
    return ret;
}

Matrix3d RespondImpl::MultiplyInterventionTransition(
    const Matrix3d &state, const DataLoader &data_loader) const {
    Matrix3d interventionTransitionProbs =
        data_loader.GetInterventionTransitionRates(time);

    Matrix3d ret = CreateMatrix3d(state.dimension(0), state.dimension(1),
                                  state.dimension(2));

    for (int i = 0;
         i < this->state.dimension(static_cast<int>(Dimension::kIntervention));
         ++i) {

        Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
        auto extentTrans = state.dimensions();
        offsetTrans[static_cast<int>(Dimension::kIntervention)] =
            i * state.dimension(static_cast<int>(Dimension::kIntervention));

        Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
        auto extentState = state.dimensions();
        offsetState[static_cast<int>(Dimension::kIntervention)] = i;
        extentState[static_cast<int>(Dimension::kIntervention)] = 1;

        auto slicedState = state.slice(offsetState, extentState);

        Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
        bcast[static_cast<int>(Dimension::kIntervention)] =
            state.dimension(static_cast<int>(Dimension::kIntervention));

        auto broadcastedTensor = slicedState.broadcast(bcast);
        auto slicedTransition =
            interventionTransitionProbs.slice(offsetTrans, extentTrans);

        ret += MultiplyUseAfterIntervention(
            broadcastedTensor * slicedTransition, i, data_loader);
    }
    return ret;
}

Matrix3d
RespondImpl::MultiplyUseAfterIntervention(const Matrix3d &interventionState,
                                          int const intervention_idx,
                                          const DataLoader &data_loader) const {
    Matrix3d result(interventionState.dimensions());
    result.setZero();
    int intervention_size =
        data_loader.GetConfig()->getStringVector("state.interventions").size();

    int behavior_size =
        data_loader.GetConfig()->getStringVector("state.ouds").size();

    auto iie = data_loader.GetInterventionInitRates();

    for (int j = 0; j < intervention_size; ++j) {

        Eigen::array<Eigen::Index, 3> result_offset = {0, 0, 0};
        Eigen::array<Eigen::Index, 3> result_extent = result.dimensions();
        result_offset[static_cast<int>(Dimension::kIntervention)] = j;
        result_extent[static_cast<int>(Dimension::kIntervention)] = 1;
        if (intervention_idx == j) {
            result.slice(result_offset, result_extent) +=
                interventionState.slice(result_offset, result_extent);
        } else {

            Matrix3d oudMat(
                (Matrix3d(result.slice(result_offset, result_extent)))
                    .dimensions());

            for (int k = 0; k < behavior_size; ++k) {
                Eigen::array<Eigen::Index, 3> intervention_offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> intervention_extent =
                    interventionState.dimensions();
                intervention_offset[static_cast<int>(
                    Dimension::kIntervention)] = j;
                intervention_extent[static_cast<int>(
                    Dimension::kIntervention)] = 1;
                intervention_offset[static_cast<int>(Dimension::kOud)] = k;
                intervention_extent[static_cast<int>(Dimension::kOud)] = 1;

                Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
                bcast[static_cast<int>(Dimension::kOud)] = behavior_size;
                auto slicedState = interventionState.slice(intervention_offset,
                                                           intervention_extent);
                auto broadcastedTensor = slicedState.broadcast(bcast);

                Eigen::array<Eigen::Index, 3> rates_offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> rates_extent = iie.dimensions();
                rates_offset[static_cast<int>(Dimension::kIntervention)] = j;
                rates_extent[static_cast<int>(Dimension::kIntervention)] = 1;
                rates_offset[static_cast<int>(Dimension::kOud)] =
                    (k * behavior_size);
                rates_extent[static_cast<int>(Dimension::kOud)] = behavior_size;

                result.slice(result_offset, result_extent) +=
                    broadcastedTensor * iie.slice(rates_offset, rates_extent);
            }
        }
    }
    return result;
}

Matrix3d RespondImpl::MultiplyFODGivenOD(const Matrix3d &state,
                                         const DataLoader &data_loader) const {
    Matrix3d fatalOverdoseMatrix = data_loader.GetFatalOverdoseRates(time);

    Matrix3d mult = fatalOverdoseMatrix * state;
    return mult;
}

Matrix3d RespondImpl::MultiplyOD(const Matrix3d &state,
                                 const DataLoader &data_loader) const {
    Matrix3d overdoseMatrix = data_loader.GetOverdoseRates(time);
    LogDebugPoint("Overdoses Matrix", overdoseMatrix);
    Matrix3d mult = overdoseMatrix * state;
    return mult;
}

Matrix3d RespondImpl::MultiplyMortality(const Matrix3d &state,
                                        const DataLoader &data_loader) const {
    Matrix3d mortalityMatrix = data_loader.GetMortalityRates();

    Matrix3d ret(state.dimensions());
    Matrix3d mor = (state * mortalityMatrix);
    return mor;
}

std::unique_ptr<Respond> Respond::Create(const std::string &log_name) {
    return std::make_unique<RespondImpl>(log_name);
}
} // namespace model
} // namespace respond
