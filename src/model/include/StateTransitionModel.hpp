#ifndef MODEL_STATETRANSITIONMODEL_HPP_
#define MODEL_STATETRANSITIONMODEL_HPP_

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

namespace data {
    class IDataBlock;
} // namespace data

namespace model {
    /// @brief Interface for all simulations
    class IStateTransitionModel {
    public:
        virtual void SetState(const std::shared_ptr<Eigen::MatrixXd> &) = 0;
        virtual std::shared_ptr<Eigen::MatrixXd> GetCurrentState() const = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        AddState(std::shared_ptr<Eigen::MatrixXd>, bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        SubtractState(std::shared_ptr<Eigen::MatrixXd>,
                      bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        MultiplyState(std::shared_ptr<Eigen::MatrixXd>,
                      bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        ScalarMultiplyState(double, bool in_place = false) = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        DivideState(double, bool in_place = false) = 0;
    };

    class StateTransitionModelFactory {
    public:
        static std::shared_ptr<IStateTransitionModel>
        MakeStateTransitionModel();
    };

} // namespace model
#endif