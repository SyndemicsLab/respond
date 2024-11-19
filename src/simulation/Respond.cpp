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

#include <Eigen/Eigen>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <execution>
#include <iostream>
#include <numeric>
#include <string>

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
            auto init_data = _data_block->GetInitialCohort();
            for (int i = 0;
                 i < _data_block->GetDemographicCombinations().size(); ++i) {
                std::shared_ptr<model::IStateTransitionModel> model = model::
                    StateTransitionModelFactory::MakeStateTransitionModel();
                auto single_dem = (*init_data)[i];
                model->SetState(single_dem);
            }
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
            MultiplyOverdoseProbabilities();
            MultiplyProbabilitiesOfFatalityGivenOverdose();
            MultiplyStandardMortalityRatios();
            MultiplyBackgroundMortalityProbabilities();
        }

        std::shared_ptr<data::ModelsVec> GetStates() const override {
            std::shared_ptr<data::ModelsVec> res =
                std::make_shared<data::ModelsVec>();
            for (auto &model : _models) {
                res->push_back(model->GetCurrentState());
            }
            return res;
        }
        std::shared_ptr<data::IRespondDataBlock> GetDataBlock() const override {
            return _data_block;
        }

    private:
        int _timestep = 0;
        std::vector<std::shared_ptr<model::IStateTransitionModel>> _models = {};
        std::shared_ptr<data::IRespondDataBlock> _data_block = nullptr;
        std::vector<data::ModelsVec> _history = {};

        void AgePopulation() const {
            // Just shift the vector accordingly
        }

        void AddMigratingCohort() const {
            auto migrating_cohort = _data_block->GetMigratingCohort();
            for (int i = 0; i < _models.size(); ++i) {
                _models[i]->AddState((*migrating_cohort)[i], true);
            }
        }
        void MultiplyBehaviorTransitions() const {
            auto transitions = _data_block->GetBehaviorTransitions();
            for (int i = 0; i < _data_block->GetBehaviors().size(); ++i) {
                for (int j = 0; j < _models.size(); ++j) {
                    auto transition_slice =
                        GetSlice(data::axis::BEHAVIOR, i, (*transitions)[j]);
                    auto final_values =
                        _models[j]->MultiplyState(transition_slice, false);
                    _models[j]->AddState(final_values, true);
                }
            }
        }
        void MultiplyInterventionTransitions() const {
            auto transitions = _data_block->GetInterventionTransitions();
            for (int i = 0; i < _data_block->GetInterventions().size(); ++i) {
                for (int j = 0; j < _models.size(); ++j) {
                    auto transition_slice = GetSlice(data::axis::INTERVENTION,
                                                     i, (*transitions)[j]);
                    auto transition_counts =
                        _models[j]->MultiplyState(transition_slice, false);
                    auto second_transition_counts =
                        MultiplyBehaviorTransitionsAfterInterventionChange(
                            i, transition_counts);
                    auto final_values = _models[j]->MultiplyState(
                        second_transition_counts, false);
                    _models[j]->AddState(final_values, true);
                }
            }
        }

        std::shared_ptr<Eigen::MatrixXd>
        MultiplyBehaviorTransitionsAfterInterventionChange(
            int intervention_idx,
            std::shared_ptr<Eigen::MatrixXd> transition_counts) const {
            // I need to rethink this
        }

        void MultiplyOverdoseProbabilities() const {
            auto od_probabilities = _data_block->GetOverdoseProbabilities();
            for (int i = 0; i < _models.size(); ++i) {
                _models[i]->MultiplyState((*od_probabilities)[i], false);
            }
        }
        void MultiplyProbabilitiesOfFatalityGivenOverdose() const {
            auto fatal_od_probabilities =
                _data_block->GetProbabilitiesOfOverdoseBeingFatal();
            for (int i = 0; i < _models.size(); ++i) {
                _models[i]->MultiplyState((*fatal_od_probabilities)[i], false);
            }
        }
        void MultiplyStandardMortalityRatios() const {
            auto smrs = _data_block->GetStandardMortalityRatios();
            for (int i = 0; i < _models.size(); ++i) {
                _models[i]->MultiplyState((*smrs)[i], false);
            }
        }
        void MultiplyBackgroundMortalityProbabilities() const {
            auto background_mortalities =
                _data_block->GetBackgroundMortalities();
            for (int i = 0; i < _models.size(); ++i) {
                _models[i]->MultiplyState((*background_mortalities)[i], false);
            }
        }

        void SnapshotHistory() {
            data::ModelsVec temp_vec = {};
            for (int i = 0; i < _models.size(); ++i) {
                temp_vec.push_back(_models[i]->GetCurrentState());
            }
            _history.push_back(temp_vec);
        }

        std::shared_ptr<Eigen::MatrixXd> GetSlice(
            data::axis dimension, int start_idx,
            const std::shared_ptr<Eigen::MatrixXd> transition_tensor) const {

            auto rows = _models[0]->GetCurrentState()->rows();
            auto cols = _models[0]->GetCurrentState()->cols();

            Eigen::MatrixXd block =
                (dimension == data::axis::INTERVENTION)
                    ? transition_tensor->block(start_idx, 0, rows, cols)
                    : transition_tensor->block(0, start_idx, rows, cols);

            std::shared_ptr<Eigen::MatrixXd> res =
                std::make_shared<Eigen::MatrixXd>(block);
            return res;
        }
    };
} // namespace simulation
