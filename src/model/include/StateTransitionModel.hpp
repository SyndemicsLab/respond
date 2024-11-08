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
        virtual bool Step() = 0;
        virtual int GetCurrentTime() const = 0;
        virtual int GetDuration() const = 0;
        virtual int LoadDatabase(std::string &const db_file) = 0;
        virtual int LoadConfig(std::string &const conf_file) = 0;
        virtual void AppendStepOperation(
            std::function<void(std::shared_ptr<data::Tensor3d>,
                               std::shared_ptr<data::IDataBlock>)>
                func) = 0;
        virtual std::vector<
            std::function<void(std::shared_ptr<data::Tensor3d>,
                               std::shared_ptr<data::IDataBlock>)>>
        GetStepOperations() const = 0;
        virtual std::vector<data::Tensor3d> GetStateHistory() const = 0;
        virtual std::shared_ptr<data::Tensor3d> GetCurrentState() const = 0;
    };

} // namespace model
#endif