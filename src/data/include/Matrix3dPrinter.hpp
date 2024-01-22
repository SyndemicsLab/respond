//===-- Matrix3dPrinter.hpp - Matrix3dPrinter class definition --*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Matrix3dPrinter class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef UTILITIES_MATRIX3DPRINTER_HPP_
#define UTILITIES_MATRIX3DPRINTER_HPP_

#include <array>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

#include "DataTypes.hpp"

namespace Data {

    /// @brief Helper Class to Cleanly Print Matrix3d Objects
    class Matrix3dPrinter {
    public:
        Matrix3dPrinter(){};

        /// @brief Static Function to Print Standard Matrix3d
        /// @param mat Matrix3d to Print
        /// @param stream Stream to Print to
        static void Print(Data::Matrix3d mat, std::ostream &stream);

        /// @brief Static Function to Print Matrix3dOverTime object
        /// @param matOverTime Matrix3dOverTime to Print
        /// @param stream Stream to Print to
        static void PrintOverTime(Data::Matrix3dOverTime matOverTime,
                                  std::ostream &stream);
    };

} // namespace Data

#endif
