////////////////////////////////////////////////////////////////////////////////
// File: Matrix3dPrinter.hpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILITIES_MATRIX3DPRINTER_HPP_
#define UTILITIES_MATRIX3DPRINTER_HPP_

#include <array>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

#include <respondsimulation/data_ops/DataTypes.hpp>

namespace Matrixify {

    /// @brief Helper Class to Cleanly Print Matrix3d Objects
    class Matrix3dPrinter {
    public:
        Matrix3dPrinter() {};

        /// @brief Static Function to Print Standard Matrix3d
        /// @param mat Matrix3d to Print
        /// @param stream Stream to Print to
        static void Print(Matrixify::Matrix3d mat, std::ostream &stream);

        /// @brief Static Function to Print Matrix4d object
        /// @param matOverTime Matrix4d to Print
        /// @param stream Stream to Print to
        static void PrintOverTime(Matrixify::Matrix4d matOverTime,
                                  std::ostream &stream);
    };

} // namespace Matrixify

#endif
