////////////////////////////////////////////////////////////////////////////////
// File: Simulation.cpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
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
#include <respond/utils/logging.hpp>

namespace respond::model {

    void RespondImpl::Run(const respond::data_ops::DataLoader &data_loader) {
        state = data_loader.GetInitialSample();
        ResetTime();
        ResetHistory();

        int duration = std::get<int>(
            data_loader.GetConfig()->get("simulation.duration", (int)0));

        for (; time < duration; ++time) {
            state = Step(data_loader);
        }
    }

    void RespondImpl::ResetHistory() {
        history.overdose_history.clear();
        history.fatal_overdose_history.clear();
        history.mortality_history.clear();
        history.intervention_admission_history.clear();
        history.state_history.clear();
    }

    void RespondImpl::LogDebugPoint(
        const std::string &message,
        const respond::data_ops::Matrix3d &matrix) const {
        respond::utils::LogDebug(logger_name,
                                 "-------------------------------------------");
        respond::utils::LogDebug(logger_name, "Timestep: " + time);
        std::stringstream ss;
        ss << message << ": " << std::endl << matrix;
        respond::utils::LogDebug(logger_name, ss.str());
        respond::utils::LogDebug(logger_name,
                                 "-------------------------------------------");
    }

    respond::data_ops::Matrix3d
    RespondImpl::Step(const respond::data_ops::DataLoader &data_loader) {
        LogDebugPoint("Starting Simulation Step", state);

        auto matrix_1 = AddEnteringCohort(state, data_loader);

        LogDebugPoint("Post Entering Sample", matrix_1);

        auto matrix_2 = MultiplyBehaviorTransition(matrix_1, data_loader);

        LogDebugPoint("Post Behavior Transition", matrix_2);

        matrix_1 = MultiplyInterventionTransition(matrix_2, data_loader);
        data_ops::Matrix3d admissions = state - matrix_1;
        data_ops::Matrix3d mat(admissions.dimensions());
        mat.setZero();
        admissions = admissions.cwiseMax(mat);
        history.intervention_admission_history[time] = admissions;

        LogDebugPoint("Post Intervention Transition", matrix_1);

        matrix_2 = MultiplyOD(matrix_1, data_loader);
        history.overdose_history[time] = matrix_2;

        auto matrix_3 = MultiplyFODGivenOD(matrix_2, data_loader);
        history.fatal_overdose_history[time] = matrix_3;

        matrix_2 = MultiplyMortality(matrix_1 - matrix_3, data_loader);
        history.mortality_history[time] = matrix_2;

        auto new_state = (matrix_1 - (matrix_2 + matrix_3));
        history.state_history[time] = new_state;

        LogDebugPoint("End of Step", new_state);

        return new_state;
    }

    respond::data_ops::Matrix3d RespondImpl::AddEnteringCohort(
        const respond::data_ops::Matrix3d &mat,
        const respond::data_ops::DataLoader &data_loader) const {
        auto es = data_loader.GetEnteringSamples().at(time);
        auto ret = state + es;
        respond::data_ops::Matrix3d roundingMatrix(es.dimensions());
        roundingMatrix.setZero();
        return ret.cwiseMax(roundingMatrix);
    }

    respond::data_ops::Matrix3d RespondImpl::MultiplyBehaviorTransition(
        const respond::data_ops::Matrix3d &state,
        const respond::data_ops::DataLoader &data_loader) const {
        auto behaviorTransitionProbs = data_loader.GetOUDTransitionRates();

        respond::data_ops::Matrix3d ret = respond::data_ops::CreateMatrix3d(
            state.dimension(0), state.dimension(1), state.dimension(2));

        for (int i = 0;
             i < state.dimension((int)respond::data_ops::Dimension::kOud);
             ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[(int)respond::data_ops::Dimension::kOud] =
                i * state.dimension((int)respond::data_ops::Dimension::kOud);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[(int)respond::data_ops::Dimension::kOud] = i;
            extentState[(int)respond::data_ops::Dimension::kOud] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[(int)respond::data_ops::Dimension::kOud] =
                state.dimension((int)respond::data_ops::Dimension::kOud);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                behaviorTransitionProbs.slice(offsetTrans, extentTrans);

            ret += (broadcastedTensor * slicedTransition);
        }
        return ret;
    }

    respond::data_ops::Matrix3d RespondImpl::MultiplyInterventionTransition(
        const respond::data_ops::Matrix3d &state,
        const respond::data_ops::DataLoader &data_loader) const {
        respond::data_ops::Matrix3d interventionTransitionProbs =
            data_loader.GetInterventionTransitionRates()[time];

        respond::data_ops::Matrix3d ret = respond::data_ops::CreateMatrix3d(
            state.dimension(0), state.dimension(1), state.dimension(2));

        for (int i = 0;
             i < this->state.dimension(
                     (int)respond::data_ops::Dimension::kIntervention);
             ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[(int)respond::data_ops::Dimension::kIntervention] =
                i * state.dimension(
                        (int)respond::data_ops::Dimension::kIntervention);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[(int)respond::data_ops::Dimension::kIntervention] = i;
            extentState[(int)respond::data_ops::Dimension::kIntervention] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[(int)respond::data_ops::Dimension::kIntervention] =
                state.dimension(
                    (int)respond::data_ops::Dimension::kIntervention);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                interventionTransitionProbs.slice(offsetTrans, extentTrans);

            ret += MultiplyUseAfterIntervention(
                broadcastedTensor * slicedTransition, i, data_loader);
        }
        return ret;
    }

    respond::data_ops::Matrix3d RespondImpl::MultiplyUseAfterIntervention(
        const respond::data_ops::Matrix3d &interventionState,
        int const intervention_idx,
        const respond::data_ops::DataLoader &data_loader) const {
        respond::data_ops::Matrix3d result(interventionState.dimensions());
        result.setZero();
        int intervention_size = data_loader.GetConfig()
                                    ->getStringVector("state.interventions")
                                    .size();

        int behavior_size =
            data_loader.GetConfig()->getStringVector("state.ouds").size();

        for (int j = 0; j < intervention_size; ++j) {

            Eigen::array<Eigen::Index, 3> result_offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> result_extent = result.dimensions();
            result_offset[(int)respond::data_ops::Dimension::kIntervention] = j;
            result_extent[(int)respond::data_ops::Dimension::kIntervention] = 1;
            if (intervention_idx == j) {
                result.slice(result_offset, result_extent) +=
                    interventionState.slice(result_offset, result_extent);
            } else {

                respond::data_ops::Matrix3d oudMat(
                    (respond::data_ops::Matrix3d(
                         result.slice(result_offset, result_extent)))
                        .dimensions());

                for (int k = 0; k < behavior_size; ++k) {
                    Eigen::array<Eigen::Index, 3> intervention_offset = {0, 0,
                                                                         0};
                    Eigen::array<Eigen::Index, 3> intervention_extent =
                        interventionState.dimensions();
                    intervention_offset[(
                        int)respond::data_ops::Dimension::kIntervention] = j;
                    intervention_extent[(
                        int)respond::data_ops::Dimension::kIntervention] = 1;
                    intervention_offset[(
                        int)respond::data_ops::Dimension::kOud] = k;
                    intervention_extent[(
                        int)respond::data_ops::Dimension::kOud] = 1;

                    Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
                    bcast[(int)respond::data_ops::Dimension::kOud] =
                        behavior_size;
                    auto slicedState = interventionState.slice(
                        intervention_offset, intervention_extent);
                    auto broadcastedTensor = slicedState.broadcast(bcast);

                    Eigen::array<Eigen::Index, 3> rates_offset = {0, 0, 0};
                    Eigen::array<Eigen::Index, 3> rates_extent =
                        data_loader.GetInterventionInitRates().dimensions();
                    rates_offset[(
                        int)respond::data_ops::Dimension::kIntervention] = j;
                    rates_extent[(
                        int)respond::data_ops::Dimension::kIntervention] = 1;
                    rates_offset[(int)respond::data_ops::Dimension::kOud] =
                        (k * behavior_size);
                    rates_extent[(int)respond::data_ops::Dimension::kOud] =
                        behavior_size;

                    result.slice(result_offset, result_extent) +=
                        broadcastedTensor *
                        data_loader.GetInterventionInitRates().slice(
                            rates_offset, rates_extent);
                }
            }
        }
        return result;
    }

    respond::data_ops::Matrix3d RespondImpl::MultiplyFODGivenOD(
        const respond::data_ops::Matrix3d &state,
        const respond::data_ops::DataLoader &data_loader) const {
        respond::data_ops::Matrix3d fatalOverdoseMatrix =
            data_loader.GetFatalOverdoseRates()[time];

        respond::data_ops::Matrix3d mult = fatalOverdoseMatrix * state;
        return mult;
    }

    respond::data_ops::Matrix3d RespondImpl::MultiplyOD(
        const respond::data_ops::Matrix3d &state,
        const respond::data_ops::DataLoader &data_loader) const {
        respond::data_ops::Matrix3d overdoseMatrix =
            data_loader.GetOverdoseRates()[time];

        respond::data_ops::Matrix3d mult = overdoseMatrix * state;
        return mult;
    }

    respond::data_ops::Matrix3d RespondImpl::MultiplyMortality(
        const respond::data_ops::Matrix3d &state,
        const respond::data_ops::DataLoader &data_loader) const {
        respond::data_ops::Matrix3d mortalityMatrix =
            data_loader.GetMortalityRates();

        respond::data_ops::Matrix3d ret(state.dimensions());
        respond::data_ops::Matrix3d mor = (state * mortalityMatrix);
        return mor;
    }
} // namespace respond::model
