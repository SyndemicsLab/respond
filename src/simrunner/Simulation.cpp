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

#include <Eigen/Eigen>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

namespace Simulation {

    Sim::Sim() : Sim::Sim(0, 0, 0, 0) {}

    Sim::Sim(int duration, int numOUDStates, int numInterventions,
             int numDemographics) {

        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        this->Duration = duration;
        this->currentTime = 0;
        this->numOUDStates = numOUDStates;
        this->numInterventions = numInterventions;
        this->numDemographics = numDemographics;

        this->numDemographicCombos = numDemographics;

        this->state = Matrixify::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions, this->numDemographics);
        this->state.setZero();
        this->transition = Matrixify::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions, this->numDemographics);
        this->transition.setZero();
        this->agingSwitch = false;
    }

    Sim::Sim(std::shared_ptr<Matrixify::IDataLoader> dataLoader) {
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        this->Duration = dataLoader->getDuration();
        this->currentTime = 0;
        this->numOUDStates = dataLoader->getNumOUDStates();
        this->numInterventions = dataLoader->getNumInterventions();
        this->numDemographics = dataLoader->getNumDemographics();
        this->numDemographicCombos = dataLoader->getNumDemographicCombos();
        this->state = Matrixify::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions,
            this->numDemographicCombos);
        this->state.setZero();
        this->transition = Matrixify::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions,
            this->numDemographicCombos);
        this->transition.setZero();
        this->agingInterval = dataLoader->getAgingInterval();
        this->ageGroupShift = dataLoader->getAgeGroupShift();

        this->Load(dataLoader);
        // if the aging interval is non-zero, activate aging
        if (this->agingInterval) {
            this->agingSwitch = true;
        }
    }

    void Sim::loadInitialSample(Matrixify::Matrix3d const &initialSample) {
        this->state = initialSample;
    }

    void Sim::loadEnteringSamples(Matrixify::Matrix4d const &enteringSamples) {
        this->enteringSamples = enteringSamples;
    }

    void
    Sim::loadOUDTransitionRates(Matrixify::Matrix3d const &oudTransitionRates) {
        this->oudTransitionRates = oudTransitionRates;
    }

    void Sim::loadInterventionInitRates(
        Matrixify::Matrix3d const &interventionInitRates) {
        this->interventionInitRates = interventionInitRates;
    }

    void Sim::loadInterventionTransitionRates(
        Matrixify::Matrix4d const &interventionTransitionRates) {
        this->interventionTransitionRates = interventionTransitionRates;
    }

    void
    Sim::loadFatalOverdoseRates(Matrixify::Matrix4d const &fatalOverdoseRates) {
        this->fatalOverdoseRates = fatalOverdoseRates;
    }

    void Sim::loadOverdoseRates(Matrixify::Matrix4d const &overdoseRates) {
        this->overdoseRates = overdoseRates;
    }

    void Sim::loadMortalityRates(Matrixify::Matrix3d const &mortalityRates) {
        this->mortalityRates = mortalityRates;
    }

    void Sim::Load(std::shared_ptr<Matrixify::IDataLoader> const dataLoader) {
        this->loadInitialSample(dataLoader->getInitialSample());
        this->loadEnteringSamples(dataLoader->getEnteringSamples());
        this->loadOUDTransitionRates(dataLoader->getOUDTransitionRates());
        this->loadInterventionInitRates(dataLoader->getInterventionInitRates());
        this->loadInterventionTransitionRates(
            dataLoader->getInterventionTransitionRates());
        this->loadFatalOverdoseRates(dataLoader->getFatalOverdoseRates());
        this->loadOverdoseRates(dataLoader->getOverdoseRates());
        this->loadMortalityRates(dataLoader->getMortalityRates());
    }

    void Sim::LoadAgingParameters(int const &shift, int const &interval) {
        this->ageGroupShift = shift;
        this->agingInterval = interval;
        this->agingSwitch = true;
    }

    void Sim::LoadTransitionModules(
        Matrixify::Matrix4d const &enteringSamples,
        Matrixify::Matrix3d const &oudTransitionRates,
        Matrixify::Matrix3d const &interventionInitRates,
        Matrixify::Matrix4d const &interventionTransitionRates,
        Matrixify::Matrix4d const &fatalOverdoseRates,
        Matrixify::Matrix4d const &overdoseRates,
        Matrixify::Matrix3d const &mortalityRates) {
        this->loadEnteringSamples(enteringSamples);
        this->loadOUDTransitionRates(oudTransitionRates);
        this->loadInterventionInitRates(interventionInitRates);
        this->loadInterventionTransitionRates(interventionTransitionRates);
        this->loadFatalOverdoseRates(fatalOverdoseRates);
        this->loadOverdoseRates(overdoseRates);
        this->loadMortalityRates(mortalityRates);
    }

    void Sim::Run() {
        Matrixify::Matrix3d zeroMat = Matrixify::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions,
            this->numDemographicCombos);
        this->history.overdoseHistory.insert(zeroMat, 0);
        this->history.fatalOverdoseHistory.insert(zeroMat, 0);
        this->history.mortalityHistory.insert(zeroMat, 0);
        this->history.stateHistory.insert(this->state, 0);
        this->history.interventionAdmissionHistory.insert(zeroMat, 0);

        for (this->currentTime = 0; this->currentTime < this->Duration;
             this->currentTime++) {
            this->state = this->step();
            this->history.stateHistory.insert(this->state, currentTime + 1);
        }
    }

    void Sim::raisePopulationAge() {
        Matrixify::Matrix3d tempState = this->state;
        Matrixify::Matrix3d::Dimensions dims = tempState.dimensions();
        for (int intervention = 0; intervention < dims[Matrixify::INTERVENTION];
             ++intervention) {
            for (int oud = 0; oud < dims[Matrixify::OUD]; ++oud) {
                // zero out the youngest age bracket
                for (int demos = 0; demos < this->ageGroupShift; ++demos) {
                    this->state(intervention, oud, demos) = 0;
                }
                // move all remaining states
                for (int demos = 0;
                     demos <
                     (dims[Matrixify::DEMOGRAPHIC_COMBO] - this->ageGroupShift);
                     ++demos) {
                    this->state(intervention, oud,
                                demos + this->ageGroupShift) =
                        tempState(intervention, oud, demos);
                }
            }
        }
    }

    Matrixify::Matrix3d Sim::step() {
        // AGING
        // needed for the half-cycle correction
        int agingReference = std::floor(this->agingInterval / 2);
        // raise cohort age at appropriate timesteps, if allowed
        if (this->agingSwitch &&
            ((this->currentTime - agingReference) % this->agingInterval == 0)) {
            this->raisePopulationAge();
        }

        // STATE TRANSITION
        Matrixify::Matrix3d enterSampleState =
            this->addEnteringSamples(this->state);

        Matrixify::Matrix3d oudTransState =
            this->multiplyOUDTransitions(enterSampleState);

        Matrixify::Matrix3d transitionedState =
            this->multiplyInterventionTransitions(oudTransState);

        this->history.interventionAdmissionHistory.insert(transitionedState,
                                                          currentTime + 1);

        Matrixify::Matrix3d overdoses =
            this->multiplyOverdoseTransitions(transitionedState);

        Matrixify::Matrix3d fatalOverdoses =
            this->multiplyFatalOverdoseTransitions(overdoses);

        Matrixify::Matrix3d mortalities = this->multiplyMortalityTransitions(
            transitionedState - fatalOverdoses);

        return (transitionedState - (mortalities + fatalOverdoses));
    }

    Matrixify::Matrix3d Sim::addEnteringSamples(Matrixify::Matrix3d state) {
        Matrixify::Matrix3d es = this->enteringSamples(this->currentTime);
        ASSERTM(es.dimensions() == state.dimensions(),
                "Entering Sample Dimensions is Correct");

        // Slice is done because I need a copy of the state instead of the
        // actual state reference
        Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
        Eigen::array<Eigen::Index, 3> extent = {state.dimensions()};
        Matrixify::Matrix3d ret = state.slice(offset, extent);

        ret += es;

        Matrixify::Matrix3d roundingMatrix(ret.dimensions());
        roundingMatrix.setZero();
        ret = ret.cwiseMax(roundingMatrix);

        return ret;
    }

    Matrixify::Matrix3d Sim::multiplyOUDTransitions(Matrixify::Matrix3d state) {
        return this->multiplyTransitions(state, Matrixify::OUD);
    }

    Matrixify::Matrix3d
    Sim::multiplyInterventionTransitions(Matrixify::Matrix3d state) {
        return this->multiplyTransitions(state, Matrixify::INTERVENTION);
    }

    Matrixify::Matrix3d Sim::getTransitionFromDim(Matrixify::Dimension dim) {
        switch (dim) {
        case Matrixify::OUD:
            if (this->interventionInitState) {
                return this->interventionInitRates;
            } else {
                return this->oudTransitionRates;
            }
        case Matrixify::INTERVENTION:
            return this->interventionTransitionRates(this->currentTime);
        case Matrixify::DEMOGRAPHIC_COMBO:
        default:
            return Matrixify::Matrix3dFactory::Create(
                this->numOUDStates, this->numInterventions,
                this->numDemographicCombos);
        }
    }

    Matrixify::Matrix3d Sim::multiplyTransitions(Matrixify::Matrix3d state,
                                                 Matrixify::Dimension dim) {
        Matrixify::Matrix3d ret(state.dimensions());
        ret.setZero();

        Matrixify::Matrix3d transition = this->getTransitionFromDim(dim);
        ASSERTM(transition.dimension(dim) == pow(state.dimension(dim), 2),
                "Transition Dimensions are Correct");
        ASSERTM(transition.dimension(dim) != 0,
                "Transition Dimensions are Non-Zero");
        ASSERTM(state.dimension(dim) != 0, "State Dimensions are Nonzero");

        int transitionsInState =
            transition.dimension(dim) / state.dimension(dim);

        for (int i = 0; i < transitionsInState; i++) {

            Eigen::array<Eigen::Index, 3> offsetTrans = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extentTrans = state.dimensions();
            offsetTrans[dim] = i * state.dimension(dim);

            Eigen::array<Eigen::Index, 3> offsetState = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extentState = state.dimensions();
            offsetState[dim] = i;
            extentState[dim] = 1;

            Matrixify::Matrix3d slicedState =
                state.slice(offsetState, extentState);

            Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
            bcast[dim] = state.dimension(dim);

            Matrixify::Matrix3d broadcastedTensor =
                slicedState.broadcast(bcast);
            Matrixify::Matrix3d slicedTransition =
                transition.slice(offsetTrans, extentTrans);

            if (dim == Matrixify::INTERVENTION) {
                this->interventionInitState = true;
                ret += this->multiplyInterventionInit(
                    broadcastedTensor * slicedTransition, i);
                this->interventionInitState = false;
            } else {
                ret += (broadcastedTensor * slicedTransition);
            }
        }
        return ret;
    }

    Matrixify::Matrix3d
    Sim::multiplyInterventionInit(Matrixify::Matrix3d interventionState,
                                  int i) {
        Matrixify::Matrix3d result(interventionState.dimensions());
        result.setZero();

        for (int j = 0; j < this->numInterventions; j++) {

            Eigen::array<Eigen::Index, 3> result_offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> result_extent = result.dimensions();
            result_offset[Matrixify::INTERVENTION] = j;
            result_extent[Matrixify::INTERVENTION] = 1;
            if (i == j) {
                result.slice(result_offset, result_extent) +=
                    interventionState.slice(result_offset, result_extent);
            } else {

                Matrixify::Matrix3d oudMat((Matrixify::Matrix3d(result.slice(
                                                result_offset, result_extent)))
                                               .dimensions());

                for (int k = 0; k < this->numOUDStates; k++) {
                    Eigen::array<Eigen::Index, 3> intervention_offset = {0, 0,
                                                                         0};
                    Eigen::array<Eigen::Index, 3> intervention_extent =
                        interventionState.dimensions();
                    intervention_offset[Matrixify::INTERVENTION] = j;
                    intervention_extent[Matrixify::INTERVENTION] = 1;
                    intervention_offset[Matrixify::OUD] = k;
                    intervention_extent[Matrixify::OUD] = 1;

                    Eigen::array<Eigen::Index, 3> bcast = {1, 1, 1};
                    bcast[Matrixify::OUD] = this->numOUDStates;
                    Matrixify::Matrix3d slicedState = interventionState.slice(
                        intervention_offset, intervention_extent);
                    Matrixify::Matrix3d broadcastedTensor =
                        slicedState.broadcast(bcast);

                    Eigen::array<Eigen::Index, 3> rates_offset = {0, 0, 0};
                    Eigen::array<Eigen::Index, 3> rates_extent =
                        this->interventionInitRates.dimensions();
                    rates_offset[Matrixify::INTERVENTION] = j;
                    rates_extent[Matrixify::INTERVENTION] = 1;
                    rates_offset[Matrixify::OUD] = (k * this->numOUDStates);
                    rates_extent[Matrixify::OUD] = this->numOUDStates;

                    result.slice(result_offset, result_extent) +=
                        broadcastedTensor * this->interventionInitRates.slice(
                                                rates_offset, rates_extent);
                }
            }
        }
        return result;
    }

    Matrixify::Matrix3d
    Sim::multiplyFatalOverdoseTransitions(Matrixify::Matrix3d state) {
        Matrixify::Matrix3d fatalOverdoseMatrix =
            this->fatalOverdoseRates(this->currentTime);
        ASSERTM(fatalOverdoseMatrix.dimensions() == state.dimensions(),
                "Fatal Overdose Dimensions are Correct");

        Matrixify::Matrix3d mult = fatalOverdoseMatrix * state;

        this->history.fatalOverdoseHistory.insert(mult, this->currentTime + 1);

        return mult;
    }

    Matrixify::Matrix3d
    Sim::multiplyOverdoseTransitions(Matrixify::Matrix3d state) {
        Matrixify::Matrix3d overdoseMatrix =
            this->overdoseRates(this->currentTime);

        ASSERTM(overdoseMatrix.dimensions() == state.dimensions(),
                "Overdose Dimensions equal State Dimensions");

        Matrixify::Matrix3d mult = overdoseMatrix * state;
        this->history.overdoseHistory.insert(mult, this->currentTime + 1);
        return mult;
    }

    Matrixify::Matrix3d
    Sim::multiplyMortalityTransitions(Matrixify::Matrix3d state) {
        Matrixify::Matrix3d mortalityMatrix = this->mortalityRates;

        ASSERTM(mortalityMatrix.dimensions() == state.dimensions(),
                "Mortality Dimensions equal State Dimensions");

        Matrixify::Matrix3d ret(state.dimensions());
        Matrixify::Matrix3d mor = (state * mortalityMatrix);

        this->history.mortalityHistory.insert(mor, this->currentTime + 1);
        return mor;
    }
} // namespace Simulation
