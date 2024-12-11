#include "RespondImpl.hpp"

namespace synmodels::models {
    /*! Concrete Class for Respond implementing the IRespond interface
     *  @image html png/RESPOND-StateMatrix.png
     *  @image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    RespondImpl::RespondImpl(STMODEL_POINTER model) {
        _model = (model == nullptr)
                     ? _model =
                           synmodels::kernels::StateTransitionModel::Create()
                     : model;
    }

    bool RespondImpl::Run(const RDATASTORE_POINTER &data_store) {
        _data_store = data_store;
        int duration = _data_store->GetDuration();
        std::vector<int> history_steps =
            _data_store->GetHistoryTimestepsToStore();
        _model->SetState(_data_store->GetInitialCohortState());
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

    Eigen::VectorXd const RespondImpl::GetState() const {
        return _model->GetState();
    }

    HISTORY_MAP const &RespondImpl::GetSimulationHistory() const {
        return _history;
    }

    bool RespondImpl::Step() {
        _model = AddMigratingCohort();
        _model = Transition();
        _model = MultiplyOverdoseProbabilities();
        _model = MultiplyProbabilitiesOfFatalityGivenOverdose();
        _model = MultiplyStandardMortalityRatios();
        _model = MultiplyBackgroundMortalityProbabilities();
        return true;
    }

    STMODEL_POINTER RespondImpl::AddMigratingCohort() const {
        auto migrating_cohort = _data_store->GetMigratingCohortState(timestep);
        _model->AddState(migrating_cohort, true);
        return _model;
    }

    STMODEL_POINTER RespondImpl::Transition() const {
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

    STMODEL_POINTER RespondImpl::MultiplyOverdoseProbabilities() const {
        auto od_probabilities =
            _data_store->GetOverdoseProbabilityState(timestep);
        _model->MultiplyState(od_probabilities, false);
        return _model;
    }
    STMODEL_POINTER
    RespondImpl::MultiplyProbabilitiesOfFatalityGivenOverdose() const {
        auto fatal_od_probabilities =
            _data_store->GetOverdoseBeingFatalProbabilityState(timestep);
        _model->MultiplyState(fatal_od_probabilities, false);
        return _model;
    }
    STMODEL_POINTER RespondImpl::MultiplyStandardMortalityRatios() const {
        auto smrs = _data_store->GetStandardMortalityRatioState(timestep);
        _model->MultiplyState(smrs, false);
        return _model;
    }
    STMODEL_POINTER
    RespondImpl::MultiplyBackgroundMortalityProbabilities() const {
        auto background_mortalities =
            _data_store->GetBackgroundMortalityState(timestep);
        _model->MultiplyState(background_mortalities, false);
        return _model;
    }

    void RespondImpl::SnapshotHistory() {
        _history[timestep] = _model->GetState();
    }

    std::unique_ptr<Respond> Respond::Create(STMODEL_POINTER model) {
        std::unique_ptr<Respond> res = std::make_unique<RespondImpl>(model);
        return res;
    }
} // namespace synmodels::models
