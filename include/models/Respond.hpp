#ifndef MODELS_RESPOND_HPP_
#define MODELS_RESPOND_HPP_

#include "data/RespondDataStore.hpp"
#include "kernels/StateTransitionModel.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

/// @brief Namespace defining all simulation Operations
namespace synmodels::models {
    using STMODEL_POINTER =
        std::shared_ptr<synmodels::kernels::StateTransitionModel>;
    using RDATASTORE_POINTER =
        std::shared_ptr<synmodels::data::RespondDataStore>;
    using HISTORY_MAP = std::unordered_map<int, Eigen::VectorXd>;
    class Respond {
    public:
        virtual ~Respond() = default;
        virtual bool Run(const RDATASTORE_POINTER &) = 0;

        virtual Eigen::VectorXd const GetState() const = 0;

        virtual HISTORY_MAP const &GetSimulationHistory() const = 0;

        static std::unique_ptr<Respond> Create(STMODEL_POINTER model = nullptr);
    };
} // namespace synmodels::models
#endif // MODEL_SIMULATION_HPP_
