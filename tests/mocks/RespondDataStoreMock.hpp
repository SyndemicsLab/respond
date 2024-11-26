#ifndef RESPONDDATASTOREMOCK_HPP_
#define RESPONDDATASTOREMOCK_HPP_

#include "RespondDataStore.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace data {
    class MOCKRespondDataStore : public IRespondDataStore {
    public:
        MOCK_METHOD(int, GetDuration, (), (const, override));
        MOCK_METHOD(bool, GetStoreHistoryStatus, (), (const, override));
        MOCK_METHOD((std::vector<int>), GetHistoryTimestepsToStore, (),
                    (const, override));

        MOCK_METHOD(void, SetDuration, (const int &), (override));
        MOCK_METHOD(void, SetStoreHistoryStatus, (const bool &), (override));
        MOCK_METHOD(void, SetHistoryTimestepsToStore,
                    (const std::vector<int> &), (override));

        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), GetInitialCohortState,
                    (), (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), GetMigratingCohortState,
                    (const int &), (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::MatrixXd>), GetBehaviorTransitions,
                    (const int &), (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::MatrixXd>),
                    GetInterventionTransitions, (const int &),
                    (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::MatrixXd>),
                    GetBehaviorAfterInterventionTransitions, (const int &),
                    (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>),
                    GetOverdoseProbabilityState, (const int &),
                    (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>),
                    GetOverdoseBeingFatalProbabilityState, (const int &),
                    (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>),
                    GetStandardMortalityRatioState, (const int &),
                    (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>),
                    GetBackgroundMortalityState, (const int &),
                    (const, override));

        MOCK_METHOD(void, SetInitialCohortState,
                    ((std::shared_ptr<Eigen::VectorXd>)&), (override));
        MOCK_METHOD(void, SetMigratingCohortState,
                    ((std::shared_ptr<Eigen::VectorXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetBehaviorTransitions,
                    ((std::shared_ptr<Eigen::MatrixXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetInterventionTransitions,
                    ((std::shared_ptr<Eigen::MatrixXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetBehaviorAfterInterventionTransitions,
                    ((std::shared_ptr<Eigen::MatrixXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetOverdoseProbabilityState,
                    ((std::shared_ptr<Eigen::VectorXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetOverdoseBeingFatalProbabilityState,
                    ((std::shared_ptr<Eigen::VectorXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetStandardMortalityRatioState,
                    ((std::shared_ptr<Eigen::VectorXd>)&, const int &),
                    (override));
        MOCK_METHOD(void, SetBackgroundMortalityState,
                    ((std::shared_ptr<Eigen::VectorXd>)&, const int &),
                    (override));
    };
} // namespace data

#endif