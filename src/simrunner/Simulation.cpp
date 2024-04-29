//===-- Simulation.cpp - Simulation class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Simulation class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Simulation.hpp"

#include "Matrix3dPrinter.hpp"

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

    Respond::Respond(std::shared_ptr<Matrixify::IDataLoader> dataLoader) {
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
    Respond::run(std::shared_ptr<Matrixify::IDataLoader> const &dataloader,
                 std::shared_ptr<Matrixify::ICostLoader> const &costloader,
                 std::shared_ptr<Matrixify::IUtilityLoader> const &utilloader) {
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
        Matrixify::Matrix3d zeroMat = createStandardMatrix3d();
        this->history.overdoseHistory.insert(zeroMat, this->getCurrentTime());
        this->history.fatalOverdoseHistory.insert(zeroMat,
                                                  this->getCurrentTime());
        this->history.mortalityHistory.insert(zeroMat, this->getCurrentTime());
        this->history.interventionAdmissionHistory.insert(
            zeroMat, this->getCurrentTime());
        this->history.stateHistory.insert(this->state, this->getCurrentTime());
    }

    Matrixify::Matrix3d Respond::step() {
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

        Matrixify::Matrix3d enterSampleState =
            this->addEnteringSample(this->state);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum2 = enterSampleState.sum();
        this->getLogger()->debug(
            "Post Entering Sample Population State Sum: {}", sum2(0));
#endif
        Matrixify::Matrix3d oudTransState =
            this->multBehaviorTransition(enterSampleState);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum3 = oudTransState.sum().eval();
        this->getLogger()->debug("Post OUD Transition Population State Sum: {}",
                                 sum3(0));
#endif

        Matrixify::Matrix3d transitionedState =
            this->multInterventionTransition(oudTransState);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum4 = transitionedState.sum();
        this->getLogger()->debug(
            "Post Intervention Transition Population State Sum: {}", sum4(0));
#endif

        Matrixify::Matrix3d overdoses = this->multOD(transitionedState);

        Matrixify::Matrix3d fatalOverdoses = this->multFODGivenOD(overdoses);

        Matrixify::Matrix3d mortalities =
            this->multMortality(transitionedState - fatalOverdoses);

        auto new_state = (transitionedState - (mortalities + fatalOverdoses));

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum5 = (mortalities + fatalOverdoses).sum();
        this->getLogger()->debug("Mortalities + FODs Sum: {}", sum5(0));
        Eigen::Tensor<double, 0> t = new_state.sum();
        this->getLogger()->debug("Final Step Population Sum: {}", t(0));
#endif

        this->history.interventionAdmissionHistory.insert(transitionedState,
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
        extent[Matrixify::DEMOGRAPHIC_COMBO] -= shift_val;
        auto rolling_state = this->state.slice(offset, extent);
        offset[Matrixify::DEMOGRAPHIC_COMBO] += shift_val;
        this->state.slice(offset, extent) = rolling_state;
        offset = {0, 0, 0};
        extent = {state.dimensions()};
        extent[Matrixify::DEMOGRAPHIC_COMBO] = shift_val;
        this->state.slice(offset, extent).setConstant(0);
    }

    Matrixify::Matrix3d
    Respond::addEnteringSample(Matrixify::Matrix3d const mat) const {
        Matrixify::Matrix3d es =
            this->getDataLoader()->getEnteringSamples().getMatrix3dAtTimestep(
                this->currentTime);
        ASSERTM(es.dimensions() == this->state.dimensions(),
                "Entering Sample Dimensions is Correct");
        auto ret = this->state + es;
        Matrixify::Matrix3d roundingMatrix(es.dimensions());
        roundingMatrix.setZero();
        return ret.cwiseMax(roundingMatrix);
    }

    Matrixify::Matrix3d
    Respond::multBehaviorTransition(Matrixify::Matrix3d const state) const {
        Matrixify::Matrix3d behaviorTransitionProbs =
            this->getDataLoader()->getOUDTransitionRates();

        Matrixify::Matrix3d ret = this->createStandardMatrix3d();
        for (int i = 0; i < this->state.dimension(Matrixify::OUD); ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[Matrixify::OUD] = i * state.dimension(Matrixify::OUD);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[Matrixify::OUD] = i;
            extentState[Matrixify::OUD] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[Matrixify::OUD] = state.dimension(Matrixify::OUD);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                behaviorTransitionProbs.slice(offsetTrans, extentTrans);

            ret += (broadcastedTensor * slicedTransition);
        }
        return ret;
    }

    Matrixify::Matrix3d
    Respond::multInterventionTransition(Matrixify::Matrix3d const state) const {
        Matrixify::Matrix3d interventionTransitionProbs =
            this->getDataLoader()
                ->getInterventionTransitionRates()
                .getMatrix3dAtTimestep(this->currentTime);

        Matrixify::Matrix3d ret = this->createStandardMatrix3d();

        for (int i = 0; i < this->state.dimension(Matrixify::INTERVENTION);
             ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[Matrixify::INTERVENTION] =
                i * state.dimension(Matrixify::INTERVENTION);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[Matrixify::INTERVENTION] = i;
            extentState[Matrixify::INTERVENTION] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[Matrixify::INTERVENTION] =
                state.dimension(Matrixify::INTERVENTION);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                interventionTransitionProbs.slice(offsetTrans, extentTrans);

            ret += this->multUseAfterIntervention(
                broadcastedTensor * slicedTransition, i);
        }
        return ret;
    }

    Matrixify::Matrix3d Respond::multUseAfterIntervention(
        Matrixify::Matrix3d const interventionState,
        int const intervention_idx) const {
        Matrixify::Matrix3d result(interventionState.dimensions());
        result.setZero();

        for (int j = 0; j < this->getDataLoader()->getNumInterventions(); j++) {

            Eigen::array<Eigen::Index, 3> result_offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> result_extent = result.dimensions();
            result_offset[Matrixify::INTERVENTION] = j;
            result_extent[Matrixify::INTERVENTION] = 1;
            if (intervention_idx == j) {
                result.slice(result_offset, result_extent) +=
                    interventionState.slice(result_offset, result_extent);
            } else {

                Matrixify::Matrix3d oudMat((Matrixify::Matrix3d(result.slice(
                                                result_offset, result_extent)))
                                               .dimensions());

                for (int k = 0; k < this->getDataLoader()->getNumOUDStates();
                     k++) {
                    Eigen::array<Eigen::Index, 3> intervention_offset = {0, 0,
                                                                         0};
                    Eigen::array<Eigen::Index, 3> intervention_extent =
                        interventionState.dimensions();
                    intervention_offset[Matrixify::INTERVENTION] = j;
                    intervention_extent[Matrixify::INTERVENTION] = 1;
                    intervention_offset[Matrixify::OUD] = k;
                    intervention_extent[Matrixify::OUD] = 1;

                    Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
                    bcast[Matrixify::OUD] =
                        this->getDataLoader()->getNumOUDStates();
                    auto slicedState = interventionState.slice(
                        intervention_offset, intervention_extent);
                    auto broadcastedTensor = slicedState.broadcast(bcast);

                    Eigen::array<Eigen::Index, 3> rates_offset = {0, 0, 0};
                    Eigen::array<Eigen::Index, 3> rates_extent =
                        this->getDataLoader()
                            ->getInterventionInitRates()
                            .dimensions();
                    rates_offset[Matrixify::INTERVENTION] = j;
                    rates_extent[Matrixify::INTERVENTION] = 1;
                    rates_offset[Matrixify::OUD] =
                        (k * this->getDataLoader()->getNumOUDStates());
                    rates_extent[Matrixify::OUD] =
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

    Matrixify::Matrix3d
    Respond::multFODGivenOD(Matrixify::Matrix3d const state) const {
        Matrixify::Matrix3d fatalOverdoseMatrix =
            this->getDataLoader()
                ->getFatalOverdoseRates()
                .getMatrix3dAtTimestep(this->currentTime);
        ASSERTM(fatalOverdoseMatrix.dimensions() == state.dimensions(),
                "Fatal Overdose Dimensions are Correct");

        Matrixify::Matrix3d mult = fatalOverdoseMatrix * state;
        return mult;
    }

    Matrixify::Matrix3d Respond::multOD(Matrixify::Matrix3d const state) const {
        Matrixify::Matrix3d overdoseMatrix =
            this->getDataLoader()->getOverdoseRates().getMatrix3dAtTimestep(
                this->currentTime);

        ASSERTM(overdoseMatrix.dimensions() == state.dimensions(),
                "Overdose Dimensions equal State Dimensions");

        Matrixify::Matrix3d mult = overdoseMatrix * state;
        return mult;
    }

    Matrixify::Matrix3d
    Respond::multMortality(Matrixify::Matrix3d const state) const {
        Matrixify::Matrix3d mortalityMatrix =
            this->getDataLoader()->getMortalityRates();

        ASSERTM(mortalityMatrix.dimensions() == state.dimensions(),
                "Mortality Dimensions equal State Dimensions");

        Matrixify::Matrix3d ret(state.dimensions());
        Matrixify::Matrix3d mor = (state * mortalityMatrix);
        return mor;
    }
} // namespace Simulation
