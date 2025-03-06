////////////////////////////////////////////////////////////////////////////////
// File: Simulation.cpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respondsimulation/model/Simulation.hpp>

#include "spdlog/fmt/ostr.h"
#include <Eigen/Eigen>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

namespace Simulation {

    Respond::Respond(std::shared_ptr<data_ops::IDataLoader> dataLoader) {
        if (!dataLoader) {
            return;
        }
        this->setData(dataLoader);
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        this->setDuration(dataLoader->getDuration());
        this->currentTime = 0;
        this->state = createStandardMatrix3d();

        this->logger = dataLoader->getLogger();

        if (!this->logger) {
            if (!spdlog::get("console")) {
                this->logger = spdlog::stdout_color_mt("console");
            } else {
                this->logger = spdlog::get("console");
            }
        }
    }

    void
    Respond::run(std::shared_ptr<data_ops::IDataLoader> const &dataloader,
                 std::shared_ptr<data_ops::ICostLoader> const &costloader,
                 std::shared_ptr<data_ops::IUtilityLoader> const &utilloader) {
        if (dataloader) {
            this->setData(dataloader);
        }
        if (costloader) {
            this->setCost(costloader);
        }
        if (utilloader) {
            this->setUtility(utilloader);
        }
        if (!this->getDataLoader()) {
            this->getLogger()->error(
                "There is no data Loaded to the Simulation!");
            return;
        }

        this->state = this->getDataLoader()->getInitialSample();
        this->currentTime = 0;
        this->setupHistory();

        while (this->getCurrentTime() < this->getDuration()) {
            this->state = this->step();
            this->currentTime++;
        }
    }

    void Respond::setupHistory() {
        data_ops::Matrix3d zeroMat = createStandardMatrix3d();
        this->history.overdoseHistory.insert(zeroMat, this->getCurrentTime());
        this->history.fatalOverdoseHistory.insert(zeroMat,
                                                  this->getCurrentTime());
        this->history.mortalityHistory.insert(zeroMat, this->getCurrentTime());
        this->history.interventionAdmissionHistory.insert(
            zeroMat, this->getCurrentTime());
        this->history.stateHistory.insert(this->state, this->getCurrentTime());
    }

    data_ops::Matrix3d Respond::step() {
#ifndef NDEBUG
        this->getLogger()->debug("Timestep: {}", this->currentTime);
        Eigen::Tensor<double, 0> sum1 = this->state.sum();
        this->getLogger()->debug("State Sum: {}", sum1(0));
#endif

        int agingReference =
            std::floor(this->getDataLoader()->getAgingInterval() / 2);
        if (((this->currentTime - agingReference) %
                 this->getDataLoader()->getAgingInterval() ==
             0)) {
            this->ageUp();
        }

        data_ops::Matrix3d enterSampleState =
            this->addEnteringSample(this->state);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum2 = enterSampleState.sum();
        this->getLogger()->debug(
            "Post Entering Sample Population State Sum: {}", sum2(0));
#endif
        data_ops::Matrix3d oudTransState =
            this->multBehaviorTransition(enterSampleState);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum3 = oudTransState.sum().eval();
        this->getLogger()->debug("Post OUD Transition Population State Sum: {}",
                                 sum3(0));
#endif

        data_ops::Matrix3d transitionedState =
            this->multInterventionTransition(oudTransState);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum4 = transitionedState.sum();
        this->getLogger()->debug(
            "Post Intervention Transition Population State Sum: {}", sum4(0));
#endif

        data_ops::Matrix3d overdoses = this->multOD(transitionedState);

        data_ops::Matrix3d fatalOverdoses = this->multFODGivenOD(overdoses);

        data_ops::Matrix3d mortalities =
            this->multMortality(transitionedState - fatalOverdoses);

        auto new_state = (transitionedState - (mortalities + fatalOverdoses));

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum5 = (mortalities + fatalOverdoses).sum();
        this->getLogger()->debug("Mortalities + FODs Sum: {}", sum5(0));
        Eigen::Tensor<double, 0> t = new_state.sum();
        this->getLogger()->debug("Final Step Population Sum: {}", t(0));
#endif

        data_ops::Matrix3d admissions = this->state - transitionedState;

        data_ops::Matrix3d mat(admissions.dimensions());
        mat.setZero();

        admissions = admissions.cwiseMax(mat);

        this->history.interventionAdmissionHistory.insert(admissions,
                                                          currentTime + 1);
        this->history.overdoseHistory.insert(overdoses, this->currentTime + 1);
        this->history.fatalOverdoseHistory.insert(fatalOverdoses,
                                                  this->currentTime + 1);
        this->history.mortalityHistory.insert(mortalities,
                                              this->currentTime + 1);
        this->history.stateHistory.insert(new_state, this->currentTime + 1);

        return new_state;
    }

    void Respond::ageUp() {
        auto tempState = this->state;
        auto dims = tempState.dimensions();
        int shift_val = this->getDataLoader()->getNumDemographicCombos() /
                        this->getDataLoader()->getAgeGroupBins().size();
        Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
        Eigen::array<Eigen::Index, 3> extent = {state.dimensions()};
        extent[data_ops::DEMOGRAPHIC_COMBO] -= shift_val;
        auto rolling_state = this->state.slice(offset, extent);
        offset[data_ops::DEMOGRAPHIC_COMBO] += shift_val;
        this->state.slice(offset, extent) = rolling_state;
        offset = {0, 0, 0};
        extent = {state.dimensions()};
        extent[data_ops::DEMOGRAPHIC_COMBO] = shift_val;
        this->state.slice(offset, extent).setConstant(0);
    }

    data_ops::Matrix3d
    Respond::addEnteringSample(data_ops::Matrix3d const mat) const {
        data_ops::Matrix3d es =
            this->getDataLoader()->getEnteringSamples().getMatrix3dAtTimestep(
                this->currentTime);
        ASSERTM(es.dimensions() == this->state.dimensions(),
                "Entering Sample Dimensions is Correct");
        auto ret = this->state + es;
        data_ops::Matrix3d roundingMatrix(es.dimensions());
        roundingMatrix.setZero();
        return ret.cwiseMax(roundingMatrix);
    }

    data_ops::Matrix3d
    Respond::multBehaviorTransition(data_ops::Matrix3d const state) const {
        data_ops::Matrix3d behaviorTransitionProbs =
            this->getDataLoader()->getOUDTransitionRates();

        data_ops::Matrix3d ret = this->createStandardMatrix3d();
        for (int i = 0; i < this->state.dimension(data_ops::OUD); ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[data_ops::OUD] = i * state.dimension(data_ops::OUD);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[data_ops::OUD] = i;
            extentState[data_ops::OUD] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[data_ops::OUD] = state.dimension(data_ops::OUD);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                behaviorTransitionProbs.slice(offsetTrans, extentTrans);

            ret += (broadcastedTensor * slicedTransition);
        }
        return ret;
    }

    data_ops::Matrix3d
    Respond::multInterventionTransition(data_ops::Matrix3d const state) const {
        data_ops::Matrix3d interventionTransitionProbs =
            this->getDataLoader()
                ->getInterventionTransitionRates()
                .getMatrix3dAtTimestep(this->currentTime);

        data_ops::Matrix3d ret = this->createStandardMatrix3d();

        for (int i = 0; i < this->state.dimension(data_ops::INTERVENTION);
             ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[data_ops::INTERVENTION] =
                i * state.dimension(data_ops::INTERVENTION);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[data_ops::INTERVENTION] = i;
            extentState[data_ops::INTERVENTION] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[data_ops::INTERVENTION] =
                state.dimension(data_ops::INTERVENTION);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                interventionTransitionProbs.slice(offsetTrans, extentTrans);

            ret += this->multUseAfterIntervention(
                broadcastedTensor * slicedTransition, i);
        }
        return ret;
    }

    data_ops::Matrix3d Respond::multUseAfterIntervention(
        data_ops::Matrix3d const interventionState,
        int const intervention_idx) const {
        data_ops::Matrix3d result(interventionState.dimensions());
        result.setZero();

        for (int j = 0; j < this->getDataLoader()->getNumInterventions(); j++) {

            Eigen::array<Eigen::Index, 3> result_offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> result_extent = result.dimensions();
            result_offset[data_ops::INTERVENTION] = j;
            result_extent[data_ops::INTERVENTION] = 1;
            if (intervention_idx == j) {
                result.slice(result_offset, result_extent) +=
                    interventionState.slice(result_offset, result_extent);
            } else {

                data_ops::Matrix3d oudMat((data_ops::Matrix3d(result.slice(
                                               result_offset, result_extent)))
                                              .dimensions());

                for (int k = 0; k < this->getDataLoader()->getNumOUDStates();
                     k++) {
                    Eigen::array<Eigen::Index, 3> intervention_offset = {0, 0,
                                                                         0};
                    Eigen::array<Eigen::Index, 3> intervention_extent =
                        interventionState.dimensions();
                    intervention_offset[data_ops::INTERVENTION] = j;
                    intervention_extent[data_ops::INTERVENTION] = 1;
                    intervention_offset[data_ops::OUD] = k;
                    intervention_extent[data_ops::OUD] = 1;

                    Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
                    bcast[data_ops::OUD] =
                        this->getDataLoader()->getNumOUDStates();
                    auto slicedState = interventionState.slice(
                        intervention_offset, intervention_extent);
                    auto broadcastedTensor = slicedState.broadcast(bcast);

                    Eigen::array<Eigen::Index, 3> rates_offset = {0, 0, 0};
                    Eigen::array<Eigen::Index, 3> rates_extent =
                        this->getDataLoader()
                            ->getInterventionInitRates()
                            .dimensions();
                    rates_offset[data_ops::INTERVENTION] = j;
                    rates_extent[data_ops::INTERVENTION] = 1;
                    rates_offset[data_ops::OUD] =
                        (k * this->getDataLoader()->getNumOUDStates());
                    rates_extent[data_ops::OUD] =
                        this->getDataLoader()->getNumOUDStates();

                    result.slice(result_offset, result_extent) +=
                        broadcastedTensor *
                        this->getDataLoader()->getInterventionInitRates().slice(
                            rates_offset, rates_extent);
                }
            }
        }
        return result;
    }

    data_ops::Matrix3d
    Respond::multFODGivenOD(data_ops::Matrix3d const state) const {
        data_ops::Matrix3d fatalOverdoseMatrix =
            this->getDataLoader()
                ->getFatalOverdoseRates()
                .getMatrix3dAtTimestep(this->currentTime);
        ASSERTM(fatalOverdoseMatrix.dimensions() == state.dimensions(),
                "Fatal Overdose Dimensions are Correct");

        data_ops::Matrix3d mult = fatalOverdoseMatrix * state;
        return mult;
    }

    data_ops::Matrix3d Respond::multOD(data_ops::Matrix3d const state) const {
        data_ops::Matrix3d overdoseMatrix =
            this->getDataLoader()->getOverdoseRates().getMatrix3dAtTimestep(
                this->currentTime);

        ASSERTM(overdoseMatrix.dimensions() == state.dimensions(),
                "Overdose Dimensions equal State Dimensions");

        data_ops::Matrix3d mult = overdoseMatrix * state;
        return mult;
    }

    data_ops::Matrix3d
    Respond::multMortality(data_ops::Matrix3d const state) const {
        data_ops::Matrix3d mortalityMatrix =
            this->getDataLoader()->getMortalityRates();

        ASSERTM(mortalityMatrix.dimensions() == state.dimensions(),
                "Mortality Dimensions equal State Dimensions");

        data_ops::Matrix3d ret(state.dimensions());
        data_ops::Matrix3d mor = (state * mortalityMatrix);
        return mor;
    }
} // namespace Simulation
