#include "StateTransitionModel.hpp"
#include "MatrixXdFactory.hpp"
#include "RespondDataBlock.hpp"
#include <Eigen/Eigen>

namespace model {
    class StateTransitionModel : public virtual IStateTransitionModel {
    public:
        StateTransitionModel(uint16_t x, uint16_t y) {
            _state = std::make_shared<Eigen::MatrixXd>(
                std::move(data::MatrixXdFactory::Create(x, y)));
        }
        StateTransitionModel() : StateTransitionModel(0, 0) {}

        void SetState(
            const std::shared_ptr<Eigen::MatrixXd> &initial_state) override {
            this->_state = std::make_shared<Eigen::MatrixXd>(*initial_state);
        }
        std::shared_ptr<Eigen::MatrixXd> GetCurrentState() const override {
            return _state;
        }
        std::shared_ptr<Eigen::MatrixXd>
        AddState(std::shared_ptr<Eigen::MatrixXd> m,
                 bool in_place = false) override {
            Eigen::MatrixXd t1 = *_state;
            Eigen::MatrixXd t2 = *m;
            Eigen::MatrixXd temp_tensor = t1 + t2;
            std::shared_ptr<Eigen::MatrixXd> temp_ptr =
                std::make_shared<Eigen::MatrixXd>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::MatrixXd>
        SubtractState(std::shared_ptr<Eigen::MatrixXd> m,
                      bool in_place = false) override {
            Eigen::MatrixXd t1 = *_state;
            Eigen::MatrixXd t2 = *m;
            Eigen::MatrixXd temp_tensor = t1 - t2;
            std::shared_ptr<Eigen::MatrixXd> temp_ptr =
                std::make_shared<Eigen::MatrixXd>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::MatrixXd>
        MultiplyState(std::shared_ptr<Eigen::MatrixXd> m,
                      bool in_place = false) override {
            Eigen::MatrixXd t1 = *_state;
            Eigen::MatrixXd t2 = *m;
            Eigen::MatrixXd temp_tensor = t1 * t2;
            std::shared_ptr<Eigen::MatrixXd> temp_ptr =
                std::make_shared<Eigen::MatrixXd>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::MatrixXd>
        ScalarMultiplyState(double scalar, bool in_place = false) override {
            Eigen::MatrixXd t1 = *_state;
            Eigen::MatrixXd temp_tensor = t1 * scalar;
            std::shared_ptr<Eigen::MatrixXd> temp_ptr =
                std::make_shared<Eigen::MatrixXd>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<Eigen::MatrixXd>
        DivideState(double scalar, bool in_place = false) override {
            Eigen::MatrixXd t1 = *_state;
            Eigen::MatrixXd temp_tensor = t1 / scalar;
            std::shared_ptr<Eigen::MatrixXd> temp_ptr =
                std::make_shared<Eigen::MatrixXd>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }

    private:
        std::shared_ptr<Eigen::MatrixXd> _state;
    };

    std::shared_ptr<IStateTransitionModel>
    StateTransitionModelFactory::MakeStateTransitionModel() {
        std::shared_ptr<IStateTransitionModel> res =
            std::make_shared<StateTransitionModel>();
        return res;
    }
} // namespace model