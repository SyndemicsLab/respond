#ifndef DATA_EIGENFACTORY_HPP_
#define DATA_EIGENFACTORY_HPP_

#include <cstdint>

/// @brief Namespace used to ensure Eigen is building and linking properly
namespace Eigen {
    template <typename _Scalar, int _Rows, int _Cols, int _Options,
              int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
} // namespace Eigen

/// @brief Namespace containing all Helper Utility Classes
namespace synmodels::data {

    /// @brief Factory to create a Matrix3d Object
    class EigenFactory {
    public:
        EigenFactory() {};
        virtual ~EigenFactory() = default;

        /// @brief Function used to generate Eigen Matrix Objects
        /// @param x Integer specifying size of first axis
        /// @param y Integer specifying size of second axis
        /// @return New Matrix of size x-by-y
        static Eigen::MatrixXd CreateMatrix(const uint16_t x, const uint16_t y);

        /// @brief Function used to generate Eigen Vector Objects
        /// @param N Integer specifying the size of the Vector
        /// @return New Vector of Length N
        static Eigen::VectorXd CreateVector(const uint16_t N);
    };

} // namespace synmodels::data

#endif
