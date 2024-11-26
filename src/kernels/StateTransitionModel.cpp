#include "StateTransitionModel.hpp"
#include "EigenFactory.hpp"
#include "RespondDataStore.hpp"
#include <Eigen/Eigen>
#include <iostream>

namespace kernels {
    class StateTransitionModel : public virtual IStateTransitionModel {
    public:
        StateTransitionModel(uint16_t N) {
            _states = std::make_shared<Eigen::VectorXd>(
                std::move(data::EigenFactory::CreateVector(N)));
            _transition_probabilities = std::make_shared<Eigen::MatrixXd>(
                std::move(data::EigenFactory::CreateMatrix(N, N)));
        }
        StateTransitionModel() : StateTransitionModel(0) {}

        void SetState(
            const std::shared_ptr<Eigen::VectorXd> &initial_states) override {
            this->_states = std::make_shared<Eigen::VectorXd>(*initial_states);
        }
        std::shared_ptr<Eigen::VectorXd> GetState() const override {
            return _states;
        }
        void
        SetTransitions(const std::shared_ptr<Eigen::MatrixXd> &t) override {
            this->_transition_probabilities =
                std::make_shared<Eigen::MatrixXd>(*t);
        }
        std::shared_ptr<Eigen::MatrixXd> GetTransitions() const override {
            return _transition_probabilities;
        }
        std::shared_ptr<Eigen::VectorXd>
        Transition(bool in_place = false) override {
            if (_transition_probabilities->cols() != _states->size()) {
                // error
                return nullptr;
            }
            Eigen::VectorXd res = (*_transition_probabilities) * (*_states);
            std::shared_ptr<Eigen::VectorXd> res_ptr =
                std::make_shared<Eigen::VectorXd>(std::move(res));
            if (in_place) {
                _states = res_ptr;
            }
            return res_ptr;
        }

        std::shared_ptr<Eigen::VectorXd>
        AddState(std::shared_ptr<Eigen::VectorXd> m,
                 bool in_place = false) override {
            Eigen::VectorXd t1 = *_states;
            Eigen::VectorXd t2 = *m;
            Eigen::VectorXd temp = t1 + t2;
            std::shared_ptr<Eigen::VectorXd> temp_ptr =
                std::make_shared<Eigen::VectorXd>(std::move(temp));
            if (in_place) {
                _states = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::VectorXd>
        SubtractState(std::shared_ptr<Eigen::VectorXd> m,
                      bool in_place = false) override {
            Eigen::VectorXd t1 = *_states;
            Eigen::VectorXd t2 = *m;
            Eigen::VectorXd temp = t1 - t2;
            std::shared_ptr<Eigen::VectorXd> temp_ptr =
                std::make_shared<Eigen::VectorXd>(std::move(temp));
            if (in_place) {
                _states = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::VectorXd>
        MultiplyState(std::shared_ptr<Eigen::VectorXd> m,
                      bool in_place = false) override {
            Eigen::VectorXd t1 = *_states;
            Eigen::VectorXd t2 = *m;
            Eigen::VectorXd temp = t1.cwiseProduct(t2);
            std::shared_ptr<Eigen::VectorXd> temp_ptr =
                std::make_shared<Eigen::VectorXd>(std::move(temp));
            if (in_place) {
                _states = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::VectorXd>
        ScalarMultiplyState(double scalar, bool in_place = false) override {
            Eigen::VectorXd t1 = *_states;
            Eigen::VectorXd temp = t1 * scalar;
            std::shared_ptr<Eigen::VectorXd> temp_ptr =
                std::make_shared<Eigen::VectorXd>(std::move(temp));
            if (in_place) {
                _states = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::VectorXd>
        DivideState(double scalar, bool in_place = false) override {
            Eigen::MatrixXd t1 = *_states;
            Eigen::MatrixXd temp = t1 / scalar;
            std::shared_ptr<Eigen::VectorXd> temp_ptr =
                std::make_shared<Eigen::VectorXd>(std::move(temp));
            if (in_place) {
                _states = temp_ptr;
            }
            return temp_ptr;
        }

    private:
        std::shared_ptr<Eigen::VectorXd> _states;
        std::shared_ptr<Eigen::MatrixXd> _transition_probabilities;
    };

    std::shared_ptr<IStateTransitionModel>
    StateTransitionModelFactory::MakeStateTransitionModel() {
        std::shared_ptr<IStateTransitionModel> res =
            std::make_shared<StateTransitionModel>();
        return res;
    }
} // namespace kernels