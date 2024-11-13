#include "StateTransitionModel.hpp"
#include "RespondDataBlock.hpp"
#include "Tensor3dFactory.hpp"

namespace model {
    std::shared_ptr<IStateTransitionModel>
    StateTransitionModelFactory::MakeStateTransitionModel() {
        std::shared_ptr<IStateTransitionModel> res =
            std::make_shared<StateTransitionModel>();
        return res;
    }
    class StateTransitionModel : public virtual IStateTransitionModel {
    public:
        StateTransitionModel(uint16_t x, uint16_t y, uint16_t z) {
            _state = std::make_shared<data::Tensor3d>(
                std::move(data::Tensor3dFactory::Create(x, y, z)));
        }
        StateTransitionModel() : StateTransitionModel(0, 0, 0) {}

        void SetState(
            const std::shared_ptr<data::Tensor3d> &initial_state) override {
            this->_state = std::make_shared<data::Tensor3d>(*initial_state);
        }

        std::shared_ptr<data::Tensor3d> GetCurrentState() const override {
            return _state;
        }

        std::shared_ptr<data::Tensor3d>
        AddState(const std::shared_ptr<data::Tensor3d> &m,
                 bool in_place = false) override {
            data::Tensor3d temp_tensor = *_state + *m;
            std::shared_ptr<data::Tensor3d> temp_ptr =
                std::make_shared<data::Tensor3d>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<data::Tensor3d>
        SubtractState(const std::shared_ptr<data::Tensor3d> &m,
                      bool in_place = false) override {
            data::Tensor3d temp_tensor = *_state - *m;
            std::shared_ptr<data::Tensor3d> temp_ptr =
                std::make_shared<data::Tensor3d>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<data::Tensor3d>
        MultiplyState(const std::shared_ptr<data::Tensor3d> &m,
                      bool in_place = false) override {
            data::Tensor3d temp_tensor = *_state * *m;
            std::shared_ptr<data::Tensor3d> temp_ptr =
                std::make_shared<data::Tensor3d>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }
        std::shared_ptr<data::Tensor3d>
        DivideState(const std::shared_ptr<data::Tensor3d> &m,
                    bool in_place = false) override {
            data::Tensor3d temp_tensor = *_state / *m;
            std::shared_ptr<data::Tensor3d> temp_ptr =
                std::make_shared<data::Tensor3d>(temp_tensor);
            if (in_place) {
                _state = temp_ptr;
            }
            return temp_ptr;
        }

    private:
        std::shared_ptr<data::Tensor3d> _state;
    };
} // namespace model