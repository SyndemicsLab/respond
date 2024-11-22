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
#include "RespondDataStore.hpp"
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
        Respond() {}

        bool AppendModel(
            const data::Models_v &model,
            const std::shared_ptr<data::IRespondDataStore> &store) override {
            _models.push_back(model);
            _data_stores.push_back(store);
        }

        bool Run() override {
            for (int i = 0; i < _data_stores.size(); ++i) {
                int duration = _data_stores[i]->GetDuration();
                std::vector<int> history_steps =
                    _data_stores[i]->GetHistoryTimestepsToStore();
                int history_idx = 0;
                for (int t = 0; t < duration; ++t) {
                    Step(t, i);
                    if (t == history_steps[history_idx]) {
                        SnapshotHistory();
                        ++history_idx;
                    }
                }
            }
        }

        bool Step(int t, int idx) override {
            std::shared_ptr<model::IStateTransitionModel> model = _models[idx];
            std::shared_ptr<data::IRespondDataStore> store = _data_stores[idx];
            model = AddMigratingCohort(model, store, t);
            model = MultiplyBehaviorTransitions(model, store, t);
            model = MultiplyInterventionTransitions(model, store, t);
            model = MultiplyOverdoseProbabilities(model, store, t);
            model =
                MultiplyProbabilitiesOfFatalityGivenOverdose(model, store, t);
            model = MultiplyStandardMortalityRatios(model, store, t);
            model = MultiplyBackgroundMortalityProbabilities(model, store, t);
        }

        data::Models_v GetModels() const override { return _models; }
        std::vector<std::shared_ptr<data::IRespondDataStore>>
        GetDataStores() const override {
            return _data_stores;
        }

        std::vector<std::vector<std::string>>
        GetDemographicCombinations() const override {
            return _demographic_combos;
        }
        std::vector<std::string> GetInterventions() const override {
            return _interventions;
        }
        std::vector<std::string> GetBehaviors() const override {
            return _behaviors;
        }

        void SetDemographicCombinations(
            const std::vector<std::vector<std::string>> &dcs) override {
            _demographic_combos = dcs;
        }
        void SetInterventions(const std::vector<std::string> &is) override {
            _interventions = is;
        }
        void SetBehaviors(const std::vector<std::string> &bs) override {
            _behaviors = bs;
        }

    private:
        std::vector<std::vector<std::string>> _demographic_combos;
        std::vector<std::string> _interventions;
        std::vector<std::string> _behaviors;
        data::Models_v _models = {};
        std::vector<std::shared_ptr<data::IRespondDataStore>> _data_stores = {};
        std::vector<data::Models_v> _history = {};

        void AgePopulation() const {
            // Just shift the vector accordingly
        }

        std::shared_ptr<model::IStateTransitionModel>
        AddMigratingCohort(std::shared_ptr<model::IStateTransitionModel> model,
                           const std::shared_ptr<data::IRespondDataStore> store,
                           const int &t) const {

            auto migrating_cohort = store->GetMigratingCohortState(t);
            model->AddState(migrating_cohort, true);
            return model;
        }
        std::shared_ptr<model::IStateTransitionModel>
        MultiplyBehaviorTransitions(
            std::shared_ptr<model::IStateTransitionModel> model,
            const std::shared_ptr<data::IRespondDataStore> store,
            const int &t) const {
            auto transitions = store->GetBehaviorTransitions(t);
            for (int i = 0; i < GetBehaviors().size(); ++i) {
                for (int j = 0; j < _models.size(); ++j) {
                    auto transition_slice = GetTransitionMatrixSlice(
                        data::axis::BEHAVIOR, i, transitions);
                    auto final_values =
                        model->MultiplyState(transition_slice, false);
                    model->AddState(final_values, true);
                }
            }
        }
        std::shared_ptr<model::IStateTransitionModel>
        MultiplyInterventionTransitions(
            std::shared_ptr<model::IStateTransitionModel> model,
            const std::shared_ptr<data::IRespondDataStore> store,
            const int &t) const {
            auto transitions = store->GetInterventionTransitions(t);
            for (int i = 0; i < GetInterventions().size(); ++i) {

                auto transition_slice = GetTransitionMatrixSlice(
                    data::axis::INTERVENTION, i, transitions);
                auto transition_counts =
                    model->MultiplyState(transition_slice, false);
                auto second_transition_counts =
                    MultiplyBehaviorTransitionsAfterInterventionChange(
                        i, transition_counts);
                auto final_values =
                    model->MultiplyState(second_transition_counts, false);
                model->AddState(final_values, true);
            }
        }

        std::shared_ptr<Eigen::MatrixXd>
        MultiplyBehaviorTransitionsAfterInterventionChange(
            int intervention_idx,
            std::shared_ptr<Eigen::MatrixXd> transition_counts) const {
            // I need to rethink this
        }

        std::shared_ptr<model::IStateTransitionModel>
        MultiplyOverdoseProbabilities(
            std::shared_ptr<model::IStateTransitionModel> model,
            const std::shared_ptr<data::IRespondDataStore> store,
            const int &t) const {
            auto od_probabilities = store->GetOverdoseProbabilityState(t);
            model->MultiplyState(od_probabilities, false);
        }
        std::shared_ptr<model::IStateTransitionModel>
        MultiplyProbabilitiesOfFatalityGivenOverdose(
            std::shared_ptr<model::IStateTransitionModel> model,
            const std::shared_ptr<data::IRespondDataStore> store,
            const int &t) const {
            auto fatal_od_probabilities =
                store->GetOverdoseBeingFatalProbabilityState(t);
            model->MultiplyState(fatal_od_probabilities, false);
        }
        std::shared_ptr<model::IStateTransitionModel>
        MultiplyStandardMortalityRatios(
            std::shared_ptr<model::IStateTransitionModel> model,
            const std::shared_ptr<data::IRespondDataStore> store,
            const int &t) const {
            auto smrs = store->GetStandardMortalityRatioState(t);
            model->MultiplyState(smrs, false);
        }
        std::shared_ptr<model::IStateTransitionModel>
        MultiplyBackgroundMortalityProbabilities(
            std::shared_ptr<model::IStateTransitionModel> model,
            const std::shared_ptr<data::IRespondDataStore> store,
            const int &t) const {
            auto background_mortalities = store->GetBackgroundMortalityState(t);
            model->MultiplyState(background_mortalities, false);
        }

        void SnapshotHistory() { _history.push_back(GetModels()); }

        std::shared_ptr<Eigen::MatrixXd> GetTransitionMatrixSlice(
            data::axis dimension, int start_idx,
            const std::shared_ptr<Eigen::MatrixXd> transition_matrix) const {
            int N = GetBehaviors().size() * GetInterventions().size();
            Eigen::MatrixXd block =
                (dimension == data::axis::INTERVENTION)
                    ? transition_matrix->block(start_idx, 0, N, N)
                    : transition_matrix->block(0, start_idx, N, N);

            std::shared_ptr<Eigen::MatrixXd> res =
                std::make_shared<Eigen::MatrixXd>(block);
            return res;
        }
    };
} // namespace simulation
