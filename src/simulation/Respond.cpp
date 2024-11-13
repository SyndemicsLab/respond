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
#include "StateTransitionModel.hpp"

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
        bool BuildModel(const std::string &db, const std::string &confile,
                        const int input_set, const int parameter_set,
                        const int start_year) override {
            _data_block = data::RespondDataBlockFactory::MakeDataBlock();
            _data_block->LoadData(input_set, parameter_set, start_year, db,
                                  confile);
            _model =
                model::StateTransitionModelFactory::MakeStateTransitionModel();
            _model->SetState(_data_block->GetInitialCohort());
            _timestep = 0;
        }
        bool Run() override {
            int duration = _data_block->GetSimulationDuration();
            std::vector<int> history_steps =
                _data_block->GetHistoryTimestepsToStore();
            int history_idx = 0;
            for (_timestep = 0; _timestep < duration; ++_timestep) {
                Step();
                if (_timestep == history_steps[history_idx]) {
                    SnapshotHistory();
                    ++history_idx;
                }
            }
        }

        bool Step() override {
            AddMigratingCohort();
            MultiplyBehaviorTransitions();
            MultiplyInterventionTransitions();
            MultiplyBehaviorTransitionsAfterInterventionChange();
            MultiplyOverdoseProbabilities();
            MultiplyProbabilitiesOfFatalityGivenOverdose();
            MultiplyStandardMortalityRatios();
            MultiplyBackgroundMortalityProbabilities();
        }

        std::shared_ptr<data::Tensor3d> GetState() const override {
            return _model->GetCurrentState();
        }
        std::shared_ptr<data::IRespondDataBlock> GetDataBlock() const override {
            return _data_block;
        }

    private:
        int _timestep = 0;
        std::shared_ptr<model::IStateTransitionModel> _model = nullptr;
        std::shared_ptr<data::IRespondDataBlock> _data_block = nullptr;
        std::vector<data::Tensor3d> _history = {};

        void AgePopulation() const {
            std::shared_ptr<data::Tensor3d> transition_state =
                _model->GetCurrentState();
            // Shift State based on age
            _model->SetState(transition_state);
        }

        void AddMigratingCohort() const {
            _model->AddState(_data_block->GetMigratingCohort(), true);
        }
        void MultiplyBehaviorTransitions() const {
            _model->MultiplyState(_data_block->GetBehaviorTransitions(), true);
        }
        void MultiplyInterventionTransitions() const {
            _model->MultiplyState(_data_block->GetInterventionTransitions(),
                                  true);
        }
        void MultiplyBehaviorTransitionsAfterInterventionChange() const {
            _model->MultiplyState(
                _data_block->GetBehaviorTransitionsAfterInterventionChange(),
                true);
        }
        void MultiplyOverdoseProbabilities() const {
            _model->MultiplyState(_data_block->GetOverdoseProbabilities(),
                                  false);
        }
        void MultiplyProbabilitiesOfFatalityGivenOverdose() const {
            _model->MultiplyState(
                _data_block->GetProbabilitiesOfOverdoseBeingFatal(), false);
        }
        void MultiplyStandardMortalityRatios() const {
            _model->MultiplyState(_data_block->GetStandardMortalityRatios(),
                                  false);
        }
        void MultiplyBackgroundMortalityProbabilities() const {
            _model->MultiplyState(_data_block->GetBackgroundMortalities(),
                                  false);
        }

        void SnapshotHistory() {
            std::shared_ptr<data::Tensor3d> temp = _model->GetCurrentState();
            _history.push_back(*temp);
        }

        matrixify::Matrix3d
        multUseAfterIntervention(matrixify::Matrix3d const mat,
                                 int const intervention_idx) const;
    };

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
