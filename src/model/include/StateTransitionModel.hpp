#ifndef MODEL_STATETRANSITIONMODEL_HPP_
#define MODEL_STATETRANSITIONMODEL_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace data {
    class Tensor3d;
    class IDataBlock;
} // namespace data

namespace model {
    /// @brief Interface for all simulations
    class IStateTransitionModel {
    public:
        virtual void SetState(const std::shared_ptr<data::Tensor3d> &) = 0;
        virtual std::shared_ptr<data::Tensor3d> GetCurrentState() const = 0;
        virtual std::shared_ptr<data::Tensor3d>
        AddState(const std::shared_ptr<data::Tensor3d> &,
                 bool in_place = false) = 0;
        virtual std::shared_ptr<data::Tensor3d>
        SubtractState(const std::shared_ptr<data::Tensor3d> &,
                      bool in_place = false) = 0;
        virtual std::shared_ptr<data::Tensor3d>
        MultiplyState(const std::shared_ptr<data::Tensor3d> &,
                      bool in_place = false) = 0;
        virtual std::shared_ptr<data::Tensor3d>
        DivideState(const std::shared_ptr<data::Tensor3d> &,
                    bool in_place = false) = 0;
    };

    class StateTransitionModelFactory {
    public:
        static std::shared_ptr<IStateTransitionModel>
        MakeStateTransitionModel();
    };

} // namespace model
#endif