#ifndef RESPONDDATASTOREMOCK_HPP_
#define RESPONDDATASTOREMOCK_HPP_

#include "data/RespondDataStore.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <Eigen/Eigen>

namespace synmodels::data {
    class MOCKRespondDataStore : public RespondDataStore {
    public:
        MOCK_METHOD(int, GetDuration, (), (const, override));
        MOCK_METHOD(bool, GetStoreHistoryStatus, (), (const, override));
        MOCK_METHOD(std::vector<int>, GetHistoryTimestepsToStore, (),
                    (const, override));

        MOCK_METHOD(void, SetDuration, (const int &x), (override));
        MOCK_METHOD(void, SetStoreHistoryStatus, (const bool &x), (override));
        MOCK_METHOD(void, SetHistoryTimestepsToStore,
                    (const std::vector<int> &x), (override));

        MOCK_METHOD((Eigen::VectorXd const &), GetInitialCohortState, (),
                    (const, override));
        MOCK_METHOD((Eigen::VectorXd const &), GetMigratingCohortState,
                    (const int &x), (const, override));
        MOCK_METHOD(Eigen::MatrixXd const &, GetBehaviorTransitions,
                    (const int &x), (const, override));
        MOCK_METHOD(Eigen::MatrixXd const &, GetInterventionTransitions,
                    (const int &x), (const, override));
        MOCK_METHOD(Eigen::MatrixXd const &,
                    GetBehaviorAfterInterventionTransitions, (const int &x),
                    (const, override));
        MOCK_METHOD((Eigen::VectorXd const &), GetOverdoseProbabilityState,
                    (const int &x), (const, override));
        MOCK_METHOD((Eigen::VectorXd const &),
                    GetOverdoseBeingFatalProbabilityState, (const int &x),
                    (const, override));
        MOCK_METHOD((Eigen::VectorXd const &), GetStandardMortalityRatioState,
                    (const int &x), (const, override));
        MOCK_METHOD((Eigen::VectorXd const &), GetBackgroundMortalityState,
                    (const int &x), (const, override));

        MOCK_METHOD(void, SetInitialCohortState, (const Eigen::VectorXd &),
                    (override));
        MOCK_METHOD(void, SetMigratingCohortState,
                    (const Eigen::VectorXd &, const int &x), (override));
        MOCK_METHOD(void, SetBehaviorTransitions,
                    (const Eigen::MatrixXd &, const int &x), (override));
        MOCK_METHOD(void, SetInterventionTransitions,
                    (const Eigen::MatrixXd &, const int &x), (override));
        MOCK_METHOD(void, SetBehaviorAfterInterventionTransitions,
                    (const Eigen::MatrixXd &, const int &x), (override));
        MOCK_METHOD(void, SetOverdoseProbabilityState,
                    (const Eigen::VectorXd &, const int &x), (override));
        MOCK_METHOD(void, SetOverdoseBeingFatalProbabilityState,
                    (const Eigen::VectorXd &, const int &x), (override));
        MOCK_METHOD(void, SetStandardMortalityRatioState,
                    (const Eigen::VectorXd &, const int &x), (override));
        MOCK_METHOD(void, SetBackgroundMortalityState,
                    (const Eigen::VectorXd &, const int &x), (override));
    };
} // namespace synmodels::data

#endif