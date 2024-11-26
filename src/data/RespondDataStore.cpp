#include "RespondDataStore.hpp"
#include <Eigen/Eigen>
#include <cmath>
#include <string>

namespace data {
    class RespondDataStore : public virtual IRespondDataStore {
    public:
        RespondDataStore() {}

        int GetDuration() const override { return _duration; }

        bool GetStoreHistoryStatus() const override { return _track_history; }
        std::vector<int> GetHistoryTimestepsToStore() const override {
            return _history_timesteps;
        }

        void SetDuration(const int &d) override { _duration = d; }
        void SetStoreHistoryStatus(const bool &s) override {
            _track_history = s;
        }
        void SetHistoryTimestepsToStore(const std::vector<int> &ts) override {
            _history_timesteps = ts;
        }

        std::shared_ptr<Eigen::VectorXd>
        GetInitialCohortState() const override {
            return _initial_cohort_state;
        }
        std::shared_ptr<Eigen::VectorXd>
        GetMigratingCohortState(const int &timestep) const override {
            return _migrating_cohort_state;
        }

        std::shared_ptr<Eigen::MatrixXd>
        GetBehaviorTransitions(const int &timestep) const override {
            return _behavior_transitions;
        }

        std::shared_ptr<Eigen::MatrixXd>
        GetInterventionTransitions(const int &timestep) const override {
            return _intervention_transitions;
        }

        std::shared_ptr<Eigen::MatrixXd>
        GetBehaviorAfterInterventionTransitions(
            const int &timestep) const override {
            return _behavior_after_intervention_change_transitions;
        }

        std::shared_ptr<Eigen::VectorXd>
        GetOverdoseProbabilityState(const int &timestep) const override {
            return _overdose_probability_state;
        }

        std::shared_ptr<Eigen::VectorXd> GetOverdoseBeingFatalProbabilityState(
            const int &timestep) const override {
            return _overdose_being_fatal_probability_state;
        }

        std::shared_ptr<Eigen::VectorXd>
        GetStandardMortalityRatioState(const int &timestep) const override {
            return _smr_state;
        }

        std::shared_ptr<Eigen::VectorXd>
        GetBackgroundMortalityState(const int &timestep) const override {
            return _background_mortality_state;
        }

        void
        SetInitialCohortState(std::shared_ptr<Eigen::VectorXd> &s) override {
            _initial_cohort_state = s;
        }
        void SetMigratingCohortState(std::shared_ptr<Eigen::VectorXd> &s,
                                     const int &timestep) override {
            _migrating_cohort_state = s;
        }
        void SetBehaviorTransitions(std::shared_ptr<Eigen::MatrixXd> &p,
                                    const int &timestep) override {
            _behavior_transitions = p;
        }
        void SetInterventionTransitions(std::shared_ptr<Eigen::MatrixXd> &p,
                                        const int &timestep) override {
            _intervention_transitions = p;
        }
        void SetBehaviorAfterInterventionTransitions(
            std::shared_ptr<Eigen::MatrixXd> &p, const int &timestep) override {
            _behavior_after_intervention_change_transitions = p;
        }
        void SetOverdoseProbabilityState(std::shared_ptr<Eigen::VectorXd> &s,
                                         const int &timestep) override {
            _overdose_probability_state = s;
        }
        void SetOverdoseBeingFatalProbabilityState(
            std::shared_ptr<Eigen::VectorXd> &s, const int &timestep) override {
            _overdose_being_fatal_probability_state = s;
        }
        void SetStandardMortalityRatioState(std::shared_ptr<Eigen::VectorXd> &s,
                                            const int &timestep) override {
            _smr_state = s;
        }
        void SetBackgroundMortalityState(std::shared_ptr<Eigen::VectorXd> &s,
                                         const int &timestep) override {
            _background_mortality_state = s;
        }

    private:
        int _duration;
        bool _track_history;
        std::vector<int> _history_timesteps;

        std::shared_ptr<Eigen::VectorXd> _initial_cohort_state;
        std::shared_ptr<Eigen::VectorXd> _migrating_cohort_state;
        std::shared_ptr<Eigen::MatrixXd> _behavior_transitions;
        std::shared_ptr<Eigen::MatrixXd> _intervention_transitions;
        std::shared_ptr<Eigen::MatrixXd>
            _behavior_after_intervention_change_transitions;
        std::shared_ptr<Eigen::VectorXd> _overdose_probability_state;
        std::shared_ptr<Eigen::VectorXd>
            _overdose_being_fatal_probability_state;
        std::shared_ptr<Eigen::VectorXd> _smr_state;
        std::shared_ptr<Eigen::VectorXd> _background_mortality_state;

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
    };

    std::shared_ptr<IRespondDataStore>
    RespondDataStoreFactory::MakeDataStore() {
        std::shared_ptr<IRespondDataStore> res =
            std::make_shared<RespondDataStore>();
        return res;
    }
} // namespace data