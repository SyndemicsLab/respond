#include "RespondDataStoreImpl.hpp"
#include <cmath>
#include <string>

namespace synmodels::data {

    RespondDataStoreImpl::RespondDataStoreImpl() {}

    int RespondDataStoreImpl::GetDuration() const { return _duration; }

    bool RespondDataStoreImpl::GetStoreHistoryStatus() const {
        return _track_history;
    }
    std::vector<int> RespondDataStoreImpl::GetHistoryTimestepsToStore() const {
        return _history_timesteps;
    }

    void RespondDataStoreImpl::SetDuration(const int &d) { _duration = d; }
    void RespondDataStoreImpl::SetStoreHistoryStatus(const bool &s) {
        _track_history = s;
    }
    void RespondDataStoreImpl::SetHistoryTimestepsToStore(
        const std::vector<int> &ts) {
        _history_timesteps = ts;
    }

    Eigen::VectorXd const &RespondDataStoreImpl::GetInitialCohortState() const {
        return _initial_cohort_state;
    }
    Eigen::VectorXd const &
    RespondDataStoreImpl::GetMigratingCohortState(const int &timestep) const {
        return _migrating_cohort_state;
    }

    Eigen::MatrixXd const &
    RespondDataStoreImpl::GetBehaviorTransitions(const int &timestep) const {
        return _behavior_transitions;
    }

    Eigen::MatrixXd const &RespondDataStoreImpl::GetInterventionTransitions(
        const int &timestep) const {
        return _intervention_transitions;
    }

    Eigen::MatrixXd const &
    RespondDataStoreImpl::GetBehaviorAfterInterventionTransitions(
        const int &timestep) const {
        return _behavior_after_intervention_change_transitions;
    }

    Eigen::VectorXd const &RespondDataStoreImpl::GetOverdoseProbabilityState(
        const int &timestep) const {
        return _overdose_probability_state;
    }

    Eigen::VectorXd const &
    RespondDataStoreImpl::GetOverdoseBeingFatalProbabilityState(
        const int &timestep) const {
        return _overdose_being_fatal_probability_state;
    }

    Eigen::VectorXd const &RespondDataStoreImpl::GetStandardMortalityRatioState(
        const int &timestep) const {
        return _smr_state;
    }

    Eigen::VectorXd const &RespondDataStoreImpl::GetBackgroundMortalityState(
        const int &timestep) const {
        return _background_mortality_state;
    }

    void RespondDataStoreImpl::SetInitialCohortState(const Eigen::VectorXd &s) {
        _initial_cohort_state = s;
    }
    void RespondDataStoreImpl::SetMigratingCohortState(const Eigen::VectorXd &s,
                                                       const int &timestep) {
        _migrating_cohort_state = s;
    }
    void RespondDataStoreImpl::SetBehaviorTransitions(const Eigen::MatrixXd &p,
                                                      const int &timestep) {
        _behavior_transitions = p;
    }
    void
    RespondDataStoreImpl::SetInterventionTransitions(const Eigen::MatrixXd &p,
                                                     const int &timestep) {
        _intervention_transitions = p;
    }
    void RespondDataStoreImpl::SetBehaviorAfterInterventionTransitions(
        const Eigen::MatrixXd &p, const int &timestep) {
        _behavior_after_intervention_change_transitions = p;
    }
    void
    RespondDataStoreImpl::SetOverdoseProbabilityState(const Eigen::VectorXd &s,
                                                      const int &timestep) {
        _overdose_probability_state = s;
    }
    void RespondDataStoreImpl::SetOverdoseBeingFatalProbabilityState(
        const Eigen::VectorXd &s, const int &timestep) {
        _overdose_being_fatal_probability_state = s;
    }
    void RespondDataStoreImpl::SetStandardMortalityRatioState(
        const Eigen::VectorXd &s, const int &timestep) {
        _smr_state = s;
    }
    void
    RespondDataStoreImpl::SetBackgroundMortalityState(const Eigen::VectorXd &s,
                                                      const int &timestep) {
        _background_mortality_state = s;
    }

    // void BuildDemographicCombinations() {
    //     _demographic_combinations = {};
    //     std::vector<std::string> dem_types = GetDemographicTypes();
    //     std::vector<std::vector<std::string>> dem_list;
    //     for (std::string &dem : dem_types) {
    //         std::string key = "demographic." + dem;
    //         dem_list.push_back(GetStringVectorFromConfig(key));
    //     }

    //     std::vector<std::vector<std::string>> final_result;
    //     std::vector<std::string> running_tally;
    //     RecursiveDemographicBuilder(final_result, running_tally,
    //                                 dem_list.begin(), dem_list.end());
    //     for (std::vector<std::string> str_vec : final_result) {
    //         std::vector<std::string> group = {};
    //         for (std::string st : str_vec) {
    //             std::transform(
    //                 st.begin(), st.end(), st.begin(),
    //                 [](unsigned char c) { return std::tolower(c); });
    //             group.push_back(st);
    //         }
    //         _demographic_combinations.push_back(group);
    //     }
    // }

    // void RecursiveDemographicBuilder(
    //     std::vector<std::vector<std::string>> &final_result,
    //     std::vector<std::string> &current_result,
    //     std::vector<std::vector<std::string>>::const_iterator current_in,
    //     std::vector<std::vector<std::string>>::const_iterator final_in)
    //     const {
    //     if (current_in == final_in) {
    //         final_result.push_back(current_result);
    //         return;
    //     }
    //     const std::vector<std::string> &temp = *current_in;
    //     for (std::vector<std::string>::const_iterator it = temp.begin();
    //          it != temp.end(); it++) {
    //         current_result.push_back(*it);
    //         RecursiveDemographicBuilder(final_result, current_result,
    //                                     current_in + 1, final_in);
    //         current_result.pop_back();
    //     }
    // }

    std::shared_ptr<RespondDataStore> RespondDataStore::Create() {
        std::shared_ptr<RespondDataStore> res =
            std::make_shared<RespondDataStoreImpl>();
        return res;
    }
} // namespace synmodels::data