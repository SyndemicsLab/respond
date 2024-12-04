#ifndef DATA_EIGENFACTORY_HPP_
#define DATA_EIGENFACTORY_HPP_

#include <cstdint>

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

        /// @brief Main Static Function used to generate Matrix3d Objects
        /// @param numOUDStates Integer specifying number of OUD States
        /// @param numInterventions Integer specifying number of Interventions
        /// @param numDemographics Integer specifying number of Demographic
        /// Combinations
        /// @return New Matrix3d Object of specified dimensions
        static Eigen::MatrixXd CreateMatrix(const uint16_t x, const uint16_t y);

        static Eigen::VectorXd CreateVector(const uint16_t N);
    };

} // namespace synmodels::data

#endif
