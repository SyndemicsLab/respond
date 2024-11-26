#ifndef KERNEL_STATETRANSITIONMODEL_HPP_
#define KERNEL_STATETRANSITIONMODEL_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Eigen {
    template <typename _Scalar, int _Rows, int _Cols, int _Options,
              int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
} // namespace Eigen

namespace kernels {
    /// @brief Interface for all simulations
    class IStateTransitionModel {
    public:
        virtual void SetState(const std::shared_ptr<Eigen::VectorXd> &) = 0;
        virtual std::shared_ptr<Eigen::VectorXd> GetState() const = 0;
        virtual void
        SetTransitions(const std::shared_ptr<Eigen::MatrixXd> &) = 0;
        virtual std::shared_ptr<Eigen::MatrixXd> GetTransitions() const = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        Transition(bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        AddState(std::shared_ptr<Eigen::VectorXd>, bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        SubtractState(std::shared_ptr<Eigen::VectorXd>,
                      bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        MultiplyState(std::shared_ptr<Eigen::VectorXd>,
                      bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        ScalarMultiplyState(double, bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        DivideState(double, bool in_place = false) = 0;
    };

    class StateTransitionModelFactory {
    public:
        static std::shared_ptr<IStateTransitionModel>
        MakeStateTransitionModel();
    };

} // namespace kernels
#endif