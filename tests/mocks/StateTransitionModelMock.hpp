#ifndef STATETRANSITIONMODELMOCK_HPP_
#define STATETRANSITIONMODELMOCK_HPP_

#include "StateTransitionModel.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace kernels {
    class StateTransitionModelMock : public IStateTransitionModel {
        MOCK_METHOD(void, SetState, (const(std::shared_ptr<Eigen::VectorXd>) &),
                    (override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), GetState, (),
                    (const, override));
        MOCK_METHOD(void, SetTransitions,
                    (const(std::shared_ptr<Eigen::MatrixXd>) &), (override));
        MOCK_METHOD((std::shared_ptr<Eigen::MatrixXd>), GetTransitions, (),
                    (const, override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), Transition, (bool),
                    (override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), AddState,
                    ((std::shared_ptr<Eigen::VectorXd>), bool), (override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), SubtractState,
                    ((std::shared_ptr<Eigen::VectorXd>), bool), (override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), MultiplyState,
                    ((std::shared_ptr<Eigen::VectorXd>), bool), (override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), ScalarMultiplyState,
                    (double, bool), (override));
        MOCK_METHOD((std::shared_ptr<Eigen::VectorXd>), DivideState,
                    (double, bool), (override));
    };
} // namespace kernels

#endif