#ifndef MOCKSTATETRANSITIONMODEL_HPP_
#define MOCKSTATETRANSITIONMODEL_HPP_

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <Eigen/Eigen>
#include <synmodels/kernels/StateTransitionModel.hpp>

namespace synmodels::kernels {
    class MOCKStateTransitionModel : public StateTransitionModel {
    public:
        MOCK_METHOD(void, SetState, (const Eigen::VectorXd &), (override));
        MOCK_METHOD((Eigen::VectorXd const), GetState, (), (const, override));
        MOCK_METHOD(void, SetTransitions, (const Eigen::MatrixXd &),
                    (override));
        MOCK_METHOD((Eigen::MatrixXd const), GetTransitions, (),
                    (const, override));
        MOCK_METHOD((Eigen::VectorXd const), Transition, (bool), (override));
        MOCK_METHOD((Eigen::VectorXd const), AddState,
                    (const Eigen::VectorXd &, bool), (override));
        MOCK_METHOD((const Eigen::VectorXd), SubtractState,
                    (Eigen::VectorXd const &, bool), (override));
        MOCK_METHOD((const Eigen::VectorXd), MultiplyState,
                    (Eigen::VectorXd const &, bool), (override));
        MOCK_METHOD((const Eigen::VectorXd), ScalarMultiplyState,
                    (double, bool), (override));
        MOCK_METHOD((const Eigen::VectorXd), DivideState, (double, bool),
                    (override));
    };
} // namespace synmodels::kernels

#endif