#ifndef DATA_RESPONDDATASTOREIMPL_HPP_
#define DATA_RESPONDDATASTOREIMPL_HPP_

#include "data/RespondDataStore.hpp"

#include <Eigen/Eigen>
#include <memory>
#include <unordered_map>
#include <vector>

namespace synmodels::data {

    class RespondDataStoreImpl : public virtual RespondDataStore {
    public:
        RespondDataStoreImpl();
        ~RespondDataStoreImpl() = default;
        int GetDuration() const override;
        bool GetStoreHistoryStatus() const override;
        std::vector<int> GetHistoryTimestepsToStore() const override;
        void SetDuration(const int &d) override;
        void SetStoreHistoryStatus(const bool &s) override;
        void SetHistoryTimestepsToStore(const std::vector<int> &ts) override;
        Eigen::VectorXd const &GetInitialCohortState() const override;
        Eigen::VectorXd const &
        GetMigratingCohortState(const int &timestep) const override;
        Eigen::MatrixXd const &
        GetBehaviorTransitions(const int &timestep) const override;
        Eigen::MatrixXd const &
        GetInterventionTransitions(const int &timestep) const override;
        Eigen::MatrixXd const &GetBehaviorAfterInterventionTransitions(
            const int &timestep) const override;
        Eigen::VectorXd const &
        GetOverdoseProbabilityState(const int &timestep) const override;
        Eigen::VectorXd const &GetOverdoseBeingFatalProbabilityState(
            const int &timestep) const override;
        Eigen::VectorXd const &
        GetStandardMortalityRatioState(const int &timestep) const override;
        Eigen::VectorXd const &
        GetBackgroundMortalityState(const int &timestep) const override;
        void SetInitialCohortState(const Eigen::VectorXd &s) override;
        void SetMigratingCohortState(const Eigen::VectorXd &s,
                                     const int &timestep) override;
        void SetBehaviorTransitions(const Eigen::MatrixXd &p,
                                    const int &timestep) override;
        void SetInterventionTransitions(const Eigen::MatrixXd &p,
                                        const int &timestep) override;
        void
        SetBehaviorAfterInterventionTransitions(const Eigen::MatrixXd &p,
                                                const int &timestep) override;
        void SetOverdoseProbabilityState(const Eigen::VectorXd &s,
                                         const int &timestep) override;
        void
        SetOverdoseBeingFatalProbabilityState(const Eigen::VectorXd &s,
                                              const int &timestep) override;
        void SetStandardMortalityRatioState(const Eigen::VectorXd &s,
                                            const int &timestep) override;
        void SetBackgroundMortalityState(const Eigen::VectorXd &s,
                                         const int &timestep) override;

    private:
        int _duration;
        bool _track_history;
        std::vector<int> _history_timesteps;

        Eigen::VectorXd _initial_cohort_state;
        Eigen::VectorXd _migrating_cohort_state;
        Eigen::MatrixXd _behavior_transitions;
        Eigen::MatrixXd _intervention_transitions;
        Eigen::MatrixXd _behavior_after_intervention_change_transitions;
        Eigen::VectorXd _overdose_probability_state;
        Eigen::VectorXd _overdose_being_fatal_probability_state;
        Eigen::VectorXd _smr_state;
        Eigen::VectorXd _background_mortality_state;
    };
} // namespace synmodels::data

#endif