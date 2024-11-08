//===-- simulation.cpp - simulation class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the simulation class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Respond.hpp"
#include "RespondDataBlock.hpp"

#include "Matrix3dPrinter.hpp"

#include <Eigen/Eigen>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

namespace simulation {
    /*! Concrete Class for Respond implementing the IRespond interface
     *  @image html png/RESPOND-StateMatrix.png
     *  @image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    class Respond : public virtual IRespond {
    public:
    public:
        bool BuildModel() override {}
        bool LoadDataBlock(std::string &const) override {}
        std::shared_ptr<data::IDataBlock> GetDataBlock() const override {}
        void
        AddMigratingCohort(std::shared_ptr<data::Tensor3d> state,
                           std::shared_ptr<data::IDataBlock>) const override {}
        void MultiplyBehaviorTransitions(
            std::shared_ptr<data::Tensor3d> state,
            std::shared_ptr<data::IDataBlock>) const override {}
        void MultiplyInterventionTransitions(
            std::shared_ptr<data::Tensor3d> state,
            std::shared_ptr<data::IDataBlock>) const override {}
        void MultiplyBehaviorTransitionsAfterInterventionChange(
            std::shared_ptr<data::Tensor3d> state,
            std::shared_ptr<data::IDataBlock>) const override {}
        void MultiplyOverdoseProbabilities(
            std::shared_ptr<data::Tensor3d> state,
            std::shared_ptr<data::IDataBlock>) const override {}
        void MultiplyProbabilitiesOfFatalityGivenOverdose(
            std::shared_ptr<data::Tensor3d> state,
            std::shared_ptr<data::IDataBlock>) const override {}
        void MultiplyBackgroundMortalityProbabilities(
            std::shared_ptr<data::Tensor3d> state,
            std::shared_ptr<data::IDataBlock>) const override {}

    private:
        matrixify::Matrix3d
        multUseAfterIntervention(matrixify::Matrix3d const mat,
                                 int const intervention_idx) const;

        matrixify::Matrix3d
        addEnteringSample(matrixify::Matrix3d const mat) const;

        matrixify::Matrix3d
        multBehaviorTransition(matrixify::Matrix3d const mat) const;

        matrixify::Matrix3d
        multInterventionTransition(matrixify::Matrix3d const mat) const;

        matrixify::Matrix3d multFODGivenOD(matrixify::Matrix3d const mat) const;

        matrixify::Matrix3d multOD(matrixify::Matrix3d const mat) const;

        matrixify::Matrix3d multMortality(matrixify::Matrix3d const mat) const;

        inline matrixify::Matrix3d createStandardMatrix3d() const {
            matrixify::Matrix3d mat = matrixify::Matrix3dFactory::Create(
                dataLoader->getNumOUDStates(),
                dataLoader->getNumInterventions(),
                dataLoader->getNumDemographicCombos());
            mat.setZero();
            return mat;
        }
    };

    Respond::Respond(std::shared_ptr<matrixify::IDataLoader> dataLoader) {
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
    Respond::run(std::shared_ptr<matrixify::IDataLoader> const &dataloader,
                 std::shared_ptr<matrixify::ICostLoader> const &costloader,
                 std::shared_ptr<matrixify::IUtilityLoader> const &utilloader) {
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
                "There is no data Loaded to the simulation!");
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
        matrixify::Matrix3d zeroMat = createStandardMatrix3d();
        this->history.overdoseHistory.insert(zeroMat, this->getCurrentTime());
        this->history.fatalOverdoseHistory.insert(zeroMat,
                                                  this->getCurrentTime());
        this->history.mortalityHistory.insert(zeroMat, this->getCurrentTime());
        this->history.interventionAdmissionHistory.insert(
            zeroMat, this->getCurrentTime());
        this->history.stateHistory.insert(this->state, this->getCurrentTime());
    }

    matrixify::Matrix3d Respond::step() {
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

        matrixify::Matrix3d enterSampleState =
            this->addEnteringSample(this->state);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum2 = enterSampleState.sum();
        this->getLogger()->debug(
            "Post Entering Sample Population State Sum: {}", sum2(0));
#endif
        matrixify::Matrix3d oudTransState =
            this->multBehaviorTransition(enterSampleState);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum3 = oudTransState.sum().eval();
        this->getLogger()->debug("Post OUD Transition Population State Sum: {}",
                                 sum3(0));
#endif

        matrixify::Matrix3d transitionedState =
            this->multInterventionTransition(oudTransState);

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum4 = transitionedState.sum();
        this->getLogger()->debug(
            "Post Intervention Transition Population State Sum: {}", sum4(0));
#endif

        matrixify::Matrix3d overdoses = this->multOD(transitionedState);

        matrixify::Matrix3d fatalOverdoses = this->multFODGivenOD(overdoses);

        matrixify::Matrix3d mortalities =
            this->multMortality(transitionedState - fatalOverdoses);

        auto new_state = (transitionedState - (mortalities + fatalOverdoses));

#ifndef NDEBUG
        Eigen::Tensor<double, 0> sum5 = (mortalities + fatalOverdoses).sum();
        this->getLogger()->debug("Mortalities + FODs Sum: {}", sum5(0));
        Eigen::Tensor<double, 0> t = new_state.sum();
        this->getLogger()->debug("Final Step Population Sum: {}", t(0));
#endif

        matrixify::Matrix3d admissions = this->state - transitionedState;

        matrixify::Matrix3d mat(admissions.dimensions());
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
        extent[matrixify::DEMOGRAPHIC_COMBO] -= shift_val;
        auto rolling_state = this->state.slice(offset, extent);
        offset[matrixify::DEMOGRAPHIC_COMBO] += shift_val;
        this->state.slice(offset, extent) = rolling_state;
        offset = {0, 0, 0};
        extent = {state.dimensions()};
        extent[matrixify::DEMOGRAPHIC_COMBO] = shift_val;
        this->state.slice(offset, extent).setConstant(0);
    }

    matrixify::Matrix3d
    Respond::addEnteringSample(matrixify::Matrix3d const mat) const {
        matrixify::Matrix3d es =
            this->getDataLoader()->getEnteringSamples().getMatrix3dAtTimestep(
                this->currentTime);
        ASSERTM(es.dimensions() == this->state.dimensions(),
                "Entering Sample Dimensions is Correct");
        auto ret = this->state + es;
        matrixify::Matrix3d roundingMatrix(es.dimensions());
        roundingMatrix.setZero();
        return ret.cwiseMax(roundingMatrix);
    }

    matrixify::Matrix3d
    Respond::multBehaviorTransition(matrixify::Matrix3d const state) const {
        matrixify::Matrix3d behaviorTransitionProbs =
            this->getDataLoader()->getOUDTransitionRates();

        matrixify::Matrix3d ret = this->createStandardMatrix3d();
        for (int i = 0; i < this->state.dimension(matrixify::OUD); ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[matrixify::OUD] = i * state.dimension(matrixify::OUD);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[matrixify::OUD] = i;
            extentState[matrixify::OUD] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[matrixify::OUD] = state.dimension(matrixify::OUD);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                behaviorTransitionProbs.slice(offsetTrans, extentTrans);

            ret += (broadcastedTensor * slicedTransition);
        }
        return ret;
    }

    matrixify::Matrix3d
    Respond::multInterventionTransition(matrixify::Matrix3d const state) const {
        matrixify::Matrix3d interventionTransitionProbs =
            this->getDataLoader()
                ->getInterventionTransitionRates()
                .getMatrix3dAtTimestep(this->currentTime);

        matrixify::Matrix3d ret = this->createStandardMatrix3d();

        for (int i = 0; i < this->state.dimension(matrixify::INTERVENTION);
             ++i) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            auto extentTrans = state.dimensions();
            offsetTrans[matrixify::INTERVENTION] =
                i * state.dimension(matrixify::INTERVENTION);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            auto extentState = state.dimensions();
            offsetState[matrixify::INTERVENTION] = i;
            extentState[matrixify::INTERVENTION] = 1;

            auto slicedState = state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[matrixify::INTERVENTION] =
                state.dimension(matrixify::INTERVENTION);

            auto broadcastedTensor = slicedState.broadcast(bcast);
            auto slicedTransition =
                interventionTransitionProbs.slice(offsetTrans, extentTrans);

            ret += this->multUseAfterIntervention(
                broadcastedTensor * slicedTransition, i);
        }
        return ret;
    }

    matrixify::Matrix3d Respond::multUseAfterIntervention(
        matrixify::Matrix3d const interventionState,
        int const intervention_idx) const {
        matrixify::Matrix3d result(interventionState.dimensions());
        result.setZero();

        for (int j = 0; j < this->getDataLoader()->getNumInterventions(); j++) {

            Eigen::array<Eigen::Index, 3> result_offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> result_extent = result.dimensions();
            result_offset[matrixify::INTERVENTION] = j;
            result_extent[matrixify::INTERVENTION] = 1;
            if (intervention_idx == j) {
                result.slice(result_offset, result_extent) +=
                    interventionState.slice(result_offset, result_extent);
            } else {

                matrixify::Matrix3d oudMat((matrixify::Matrix3d(result.slice(
                                                result_offset, result_extent)))
                                               .dimensions());

                for (int k = 0; k < this->getDataLoader()->getNumOUDStates();
                     k++) {
                    Eigen::array<Eigen::Index, 3> intervention_offset = {0, 0,
                                                                         0};
                    Eigen::array<Eigen::Index, 3> intervention_extent =
                        interventionState.dimensions();
                    intervention_offset[matrixify::INTERVENTION] = j;
                    intervention_extent[matrixify::INTERVENTION] = 1;
                    intervention_offset[matrixify::OUD] = k;
                    intervention_extent[matrixify::OUD] = 1;

                    Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
                    bcast[matrixify::OUD] =
                        this->getDataLoader()->getNumOUDStates();
                    auto slicedState = interventionState.slice(
                        intervention_offset, intervention_extent);
                    auto broadcastedTensor = slicedState.broadcast(bcast);

                    Eigen::array<Eigen::Index, 3> rates_offset = {0, 0, 0};
                    Eigen::array<Eigen::Index, 3> rates_extent =
                        this->getDataLoader()
                            ->getInterventionInitRates()
                            .dimensions();
                    rates_offset[matrixify::INTERVENTION] = j;
                    rates_extent[matrixify::INTERVENTION] = 1;
                    rates_offset[matrixify::OUD] =
                        (k * this->getDataLoader()->getNumOUDStates());
                    rates_extent[matrixify::OUD] =
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

    matrixify::Matrix3d
    Respond::multFODGivenOD(matrixify::Matrix3d const state) const {
        matrixify::Matrix3d fatalOverdoseMatrix =
            this->getDataLoader()
                ->getFatalOverdoseRates()
                .getMatrix3dAtTimestep(this->currentTime);
        ASSERTM(fatalOverdoseMatrix.dimensions() == state.dimensions(),
                "Fatal Overdose Dimensions are Correct");

        matrixify::Matrix3d mult = fatalOverdoseMatrix * state;
        return mult;
    }

    matrixify::Matrix3d Respond::multOD(matrixify::Matrix3d const state) const {
        matrixify::Matrix3d overdoseMatrix =
            this->getDataLoader()->getOverdoseRates().getMatrix3dAtTimestep(
                this->currentTime);

        ASSERTM(overdoseMatrix.dimensions() == state.dimensions(),
                "Overdose Dimensions equal State Dimensions");

        matrixify::Matrix3d mult = overdoseMatrix * state;
        return mult;
    }

    matrixify::Matrix3d
    Respond::multMortality(matrixify::Matrix3d const state) const {
        matrixify::Matrix3d mortalityMatrix =
            this->getDataLoader()->getMortalityRates();

        ASSERTM(mortalityMatrix.dimensions() == state.dimensions(),
                "Mortality Dimensions equal State Dimensions");

        matrixify::Matrix3d ret(state.dimensions());
        matrixify::Matrix3d mor = (state * mortalityMatrix);
        return mor;
    }
} // namespace simulation
