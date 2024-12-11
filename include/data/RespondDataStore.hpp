#ifndef INTERFACE_DATA_RESPONDDATASTORE_HPP_
#define INTERFACE_DATA_RESPONDDATASTORE_HPP_

#include "data/EigenFactory.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

/// @brief Namespace defining data interaction with the syndemics model library
namespace synmodels::data {
    /// @brief A Class used to Maintain the Data necessary for the RESPOND Model
    class RespondDataStore {
    public:
        virtual ~RespondDataStore() = default;

        /// @brief
        /// @return
        virtual int GetDuration() const = 0;

        /// @brief
        /// @return
        virtual bool GetStoreHistoryStatus() const = 0;

        /// @brief
        /// @return
        virtual std::vector<int> GetHistoryTimestepsToStore() const = 0;

        /// @brief
        /// @param
        virtual void SetDuration(const int &) = 0;

        /// @brief
        /// @param
        virtual void SetStoreHistoryStatus(const bool &) = 0;

        /// @brief
        /// @param
        virtual void SetHistoryTimestepsToStore(const std::vector<int> &) = 0;

        /// @brief
        /// @return
        virtual Eigen::VectorXd const &GetInitialCohortState() const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::VectorXd const &
        GetMigratingCohortState(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::MatrixXd const &
        GetBehaviorTransitions(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::MatrixXd const &
        GetInterventionTransitions(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::MatrixXd const &
        GetBehaviorAfterInterventionTransitions(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::VectorXd const &
        GetOverdoseProbabilityState(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::VectorXd const &
        GetOverdoseBeingFatalProbabilityState(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::VectorXd const &
        GetStandardMortalityRatioState(const int &timestep) const = 0;

        /// @brief
        /// @param timestep
        /// @return
        virtual Eigen::VectorXd const &
        GetBackgroundMortalityState(const int &timestep) const = 0;

        /// @brief
        /// @param
        virtual void SetInitialCohortState(const Eigen::VectorXd &) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void SetMigratingCohortState(const Eigen::VectorXd &,
                                             const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void SetBehaviorTransitions(const Eigen::MatrixXd &,
                                            const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void SetInterventionTransitions(const Eigen::MatrixXd &,
                                                const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void
        SetBehaviorAfterInterventionTransitions(const Eigen::MatrixXd &,
                                                const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void SetOverdoseProbabilityState(const Eigen::VectorXd &,
                                                 const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void
        SetOverdoseBeingFatalProbabilityState(const Eigen::VectorXd &,
                                              const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void SetStandardMortalityRatioState(const Eigen::VectorXd &,
                                                    const int &timestep) = 0;

        /// @brief
        /// @param
        /// @param timestep
        virtual void SetBackgroundMortalityState(const Eigen::VectorXd &,
                                                 const int &timestep) = 0;

        /// @brief
        /// @return
        static std::unique_ptr<RespondDataStore> Create();
    };
} // namespace synmodels::data

#endif