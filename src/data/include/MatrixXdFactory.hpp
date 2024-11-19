//===-- Tensor3dFactory.hpp - Tensor3dFactory class definition --*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Tensor3dFactory class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef UTILITIES_Tensor3dFactory_HPP_
#define UTILITIES_Tensor3dFactory_HPP_

#include <cstdint>

namespace Eigen {
    template <typename _Scalar, int _Rows, int _Cols, int _Options,
              int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
} // namespace Eigen

/// @brief Namespace containing all Helper Utility Classes
namespace data {

    /// @brief Factory to create a Matrix3d Object
    class MatrixXdFactory {
    public:
        MatrixXdFactory() {};

        /// @brief Main Static Function used to generate Matrix3d Objects
        /// @param numOUDStates Integer specifying number of OUD States
        /// @param numInterventions Integer specifying number of Interventions
        /// @param numDemographics Integer specifying number of Demographic
        /// Combinations
        /// @return New Matrix3d Object of specified dimensions
        static Eigen::MatrixXd Create(const uint16_t x, const uint16_t y);
    };

} // namespace data

#endif
