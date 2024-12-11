#ifndef INTERFACE_DATA_RESPONDDATASTORE_HPP_
#define INTERFACE_DATA_RESPONDDATASTORE_HPP_

#include "data/EigenFactory.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace synmodels::data {
    class RespondDataStore {
    public:
        virtual ~RespondDataStore() = default;
        virtual int GetDuration() const = 0;
        virtual bool GetStoreHistoryStatus() const = 0;
        virtual std::vector<int> GetHistoryTimestepsToStore() const = 0;

        virtual void SetDuration(const int &) = 0;
        virtual void SetStoreHistoryStatus(const bool &) = 0;
        virtual void SetHistoryTimestepsToStore(const std::vector<int> &) = 0;

        virtual Eigen::VectorXd const &GetInitialCohortState() const = 0;
        virtual Eigen::VectorXd const &
        GetMigratingCohortState(const int &timestep) const = 0;
        virtual Eigen::MatrixXd const &
        GetBehaviorTransitions(const int &timestep) const = 0;
        virtual Eigen::MatrixXd const &
        GetInterventionTransitions(const int &timestep) const = 0;
        virtual Eigen::MatrixXd const &
        GetBehaviorAfterInterventionTransitions(const int &timestep) const = 0;
        virtual Eigen::VectorXd const &
        GetOverdoseProbabilityState(const int &timestep) const = 0;
        virtual Eigen::VectorXd const &
        GetOverdoseBeingFatalProbabilityState(const int &timestep) const = 0;
        virtual Eigen::VectorXd const &
        GetStandardMortalityRatioState(const int &timestep) const = 0;
        virtual Eigen::VectorXd const &
        GetBackgroundMortalityState(const int &timestep) const = 0;

        virtual void SetInitialCohortState(const Eigen::VectorXd &) = 0;
        virtual void SetMigratingCohortState(const Eigen::VectorXd &,
                                             const int &timestep) = 0;
        virtual void SetBehaviorTransitions(const Eigen::MatrixXd &,
                                            const int &timestep) = 0;
        virtual void SetInterventionTransitions(const Eigen::MatrixXd &,
                                                const int &timestep) = 0;
        virtual void
        SetBehaviorAfterInterventionTransitions(const Eigen::MatrixXd &,
                                                const int &timestep) = 0;
        virtual void SetOverdoseProbabilityState(const Eigen::VectorXd &,
                                                 const int &timestep) = 0;
        virtual void
        SetOverdoseBeingFatalProbabilityState(const Eigen::VectorXd &,
                                              const int &timestep) = 0;
        virtual void SetStandardMortalityRatioState(const Eigen::VectorXd &,
                                                    const int &timestep) = 0;
        virtual void SetBackgroundMortalityState(const Eigen::VectorXd &,
                                                 const int &timestep) = 0;
        static std::unique_ptr<RespondDataStore> Create();
    };
} // namespace synmodels::data

#endif