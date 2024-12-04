#ifndef KERNELS_STATETRANSITIONMODELIMPL_HPP_
#define KERNELS_STATETRANSITIONMODELIMPL_HPP_

#include "kernels/StateTransitionModel.hpp"

#include <Eigen/Eigen>

namespace synmodels::kernels {
    class StateTransitionModelImpl : public virtual StateTransitionModel {
    public:
        StateTransitionModelImpl(uint16_t N);
        StateTransitionModelImpl() : StateTransitionModelImpl(0) {}

        void SetState(const Eigen::VectorXd &initial_states) override;
        Eigen::VectorXd const GetState() const override;
        void SetTransitions(const Eigen::MatrixXd &t) override;
        Eigen::MatrixXd const GetTransitions() const override;
        Eigen::VectorXd const Transition(bool in_place = false) override;

        Eigen::VectorXd const AddState(const Eigen::VectorXd &m,
                                       bool in_place = false) override;
        Eigen::VectorXd const SubtractState(const Eigen::VectorXd &m,
                                            bool in_place = false) override;
        Eigen::VectorXd const MultiplyState(const Eigen::VectorXd &m,
                                            bool in_place = false) override;
        Eigen::VectorXd const
        ScalarMultiplyState(double scalar, bool in_place = false) override;

        Eigen::VectorXd const DivideState(double scalar,
                                          bool in_place = false) override;

    private:
        Eigen::VectorXd _state;
        Eigen::MatrixXd _transition;
    };
} // namespace synmodels::kernels

#endif