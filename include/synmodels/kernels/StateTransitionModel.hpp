#ifndef KERNELS_STATETRANSITIONMODEL_HPP_
#define KERNELS_STATETRANSITIONMODEL_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <synmodels/data/EigenFactory.hpp>
#include <vector>

namespace synmodels::kernels {
    /// @brief Interface for all simulations
    class StateTransitionModel {
    public:
        virtual ~StateTransitionModel() = default;
        virtual void SetState(const Eigen::VectorXd &) = 0;
        virtual Eigen::VectorXd const GetState() const = 0;
        virtual void SetTransitions(const Eigen::MatrixXd &) = 0;
        virtual Eigen::MatrixXd const GetTransitions() const = 0;
        virtual Eigen::VectorXd const Transition(bool in_place = false) = 0;
        virtual Eigen::VectorXd const AddState(const Eigen::VectorXd &,
                                               bool in_place = false) = 0;
        virtual Eigen::VectorXd const SubtractState(const Eigen::VectorXd &,
                                                    bool in_place = false) = 0;
        virtual Eigen::VectorXd const MultiplyState(const Eigen::VectorXd &,
                                                    bool in_place = false) = 0;
        virtual Eigen::VectorXd const
        ScalarMultiplyState(double, bool in_place = false) = 0;
        virtual Eigen::VectorXd const DivideState(double,
                                                  bool in_place = false) = 0;

        static std::unique_ptr<StateTransitionModel> Create();
    };
} // namespace synmodels::kernels
#endif