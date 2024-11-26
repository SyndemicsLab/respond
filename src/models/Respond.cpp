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
#include <string>

namespace models {
    /*! Concrete Class for Respond implementing the IRespond interface
     *  @image html png/RESPOND-StateMatrix.png
     *  @image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    class Respond : public virtual IRespond {
    public:
        Respond() {}

        bool Run() override {
            int duration = _data_store->GetDuration();
            std::vector<int> history_steps =
                _data_store->GetHistoryTimestepsToStore();
            int history_idx = 0;
            for (timestep = 0; timestep < duration; ++timestep) {
                Step();
                if (timestep == history_steps[history_idx]) {
                    SnapshotHistory();
                    ++history_idx;
                }
            }
            return true;
        }

        bool Step() override {
            _model = AddMigratingCohort();
            _model = Transition();
            _model = MultiplyOverdoseProbabilities();
            _model = MultiplyProbabilitiesOfFatalityGivenOverdose();
            _model = MultiplyStandardMortalityRatios();
            _model = MultiplyBackgroundMortalityProbabilities();
            return true;
        }

        std::shared_ptr<kernels::IStateTransitionModel>
        GetModel() const override {
            return _model;
        }
        std::shared_ptr<data::IRespondDataStore> GetDataStore() const override {
            return _data_store;
        }

        std::shared_ptr<std::vector<std::string>>
        GetInterventions() const override {
            return _interventions;
        }
        std::shared_ptr<std::vector<std::string>>
        GetBehaviors() const override {
            return _behaviors;
        }

        void SetInterventions(
            const std::shared_ptr<std::vector<std::string>> &is) override {
            _interventions = is;
        }
        void SetBehaviors(
            const std::shared_ptr<std::vector<std::string>> &bs) override {
            _behaviors = bs;
        }

    private:
        int timestep = 0;
        std::shared_ptr<std::vector<std::string>> _interventions;
        std::shared_ptr<std::vector<std::string>> _behaviors;
        std::shared_ptr<kernels::IStateTransitionModel> _model = nullptr;
        std::shared_ptr<data::IRespondDataStore> _data_store = {};
        std::vector<std::shared_ptr<kernels::IStateTransitionModel>> _history =
            {};

        std::shared_ptr<kernels::IStateTransitionModel>
        AddMigratingCohort() const {
            auto migrating_cohort =
                _data_store->GetMigratingCohortState(timestep);
            _model->AddState(migrating_cohort, true);
            return _model;
        }

        std::shared_ptr<kernels::IStateTransitionModel> Transition() const {
            // auto interventions =
            //     _data_store->GetInterventionTransitions(timestep);
            // auto diagonal = interventions->diagonal();
            // interventions->diagonal() = Eigen::VectorXd::Zero();
            // auto init_effect =
            //     _data_store->GetBehaviorAfterInterventionTransitions(timestep);
            // Eigen::MatrixXd transitions = (*interventions) * (*init_effect);
            // transitions.diagonal() = diagonal;
            // transitions *= (*_data_store->GetBehaviorTransitions(timestep));
            // std::shared_ptr<Eigen::MatrixXd> trans_ptr =
            //     std::make_shared<Eigen::MatrixXd>(transitions);
            // _model->SetTransitions(trans_ptr);
            // _model->Transition(true);
            return _model;
        }

        std::shared_ptr<kernels::IStateTransitionModel>
        MultiplyBehaviorTransitions() const {
            _model->SetTransitions(
                _data_store->GetBehaviorTransitions(timestep));
            _model->Transition(true);
            return _model;
        }
        std::shared_ptr<kernels::IStateTransitionModel>
        MultiplyInterventionTransitions() const {
            _model->SetTransitions(
                _data_store->GetInterventionTransitions(timestep));
            _model->Transition(true);
            return _model;
        }

        std::shared_ptr<kernels::IStateTransitionModel>
        MultiplyOverdoseProbabilities() const {
            auto od_probabilities =
                _data_store->GetOverdoseProbabilityState(timestep);
            _model->MultiplyState(od_probabilities, false);
            return _model;
        }
        std::shared_ptr<kernels::IStateTransitionModel>
        MultiplyProbabilitiesOfFatalityGivenOverdose() const {
            auto fatal_od_probabilities =
                _data_store->GetOverdoseBeingFatalProbabilityState(timestep);
            _model->MultiplyState(fatal_od_probabilities, false);
            return _model;
        }
        std::shared_ptr<kernels::IStateTransitionModel>
        MultiplyStandardMortalityRatios() const {
            auto smrs = _data_store->GetStandardMortalityRatioState(timestep);
            _model->MultiplyState(smrs, false);
            return _model;
        }
        std::shared_ptr<kernels::IStateTransitionModel>
        MultiplyBackgroundMortalityProbabilities() const {
            auto background_mortalities =
                _data_store->GetBackgroundMortalityState(timestep);
            _model->MultiplyState(background_mortalities, false);
            return _model;
        }

        void SnapshotHistory() { _history.push_back(GetModel()); }
    };
} // namespace models
