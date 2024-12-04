#include "StateTransitionModelImpl.hpp"
#include <Eigen/Eigen>
#include <iostream>

namespace synmodels::kernels {

    StateTransitionModelImpl::StateTransitionModelImpl(uint16_t N) {
        _state = synmodels::data::EigenFactory::CreateVector(N);
        _transition = synmodels::data::EigenFactory::CreateMatrix(N, N);
    }

    void
    StateTransitionModelImpl::SetState(const Eigen::VectorXd &initial_states) {
        this->_state = initial_states;
    }
    Eigen::VectorXd const StateTransitionModelImpl::GetState() const {
        return _state;
    }
    void StateTransitionModelImpl::SetTransitions(const Eigen::MatrixXd &t) {
        this->_transition = t;
    }
    Eigen::MatrixXd const StateTransitionModelImpl::GetTransitions() const {
        return _transition;
    }
    Eigen::VectorXd const StateTransitionModelImpl::Transition(bool in_place) {
        Eigen::VectorXd res;
        if (_transition.cols() != _state.size()) {
            // error
            return res;
        }
        res = (_transition) * (_state);
        if (in_place) {
            _state = res;
        }
        return res;
    }

    Eigen::VectorXd const
    StateTransitionModelImpl::AddState(const Eigen::VectorXd &m,
                                       bool in_place) {
        Eigen::VectorXd temp = _state + m;
        if (in_place) {
            _state = temp;
        }
        return temp;
    }
    Eigen::VectorXd const
    StateTransitionModelImpl::SubtractState(const Eigen::VectorXd &m,
                                            bool in_place) {
        Eigen::VectorXd temp = _state - m;
        if (in_place) {
            _state = temp;
        }
        return temp;
    }
    Eigen::VectorXd const
    StateTransitionModelImpl::MultiplyState(const Eigen::VectorXd &m,
                                            bool in_place) {
        Eigen::VectorXd temp = _state.cwiseProduct(m);
        if (in_place) {
            _state = temp;
        }
        return temp;
    }
    Eigen::VectorXd const
    StateTransitionModelImpl::ScalarMultiplyState(double scalar,
                                                  bool in_place) {
        Eigen::VectorXd temp = _state * scalar;
        if (in_place) {
            _state = temp;
        }
        return temp;
    }
    Eigen::VectorXd const StateTransitionModelImpl::DivideState(double scalar,
                                                                bool in_place) {
        Eigen::MatrixXd temp = _state / scalar;
        if (in_place) {
            _state = temp;
        }
        return temp;
    }

    std::shared_ptr<StateTransitionModel> StateTransitionModel::Create() {
        std::shared_ptr<StateTransitionModel> res =
            std::make_shared<StateTransitionModelImpl>();
        return res;
    }
} // namespace synmodels::kernels