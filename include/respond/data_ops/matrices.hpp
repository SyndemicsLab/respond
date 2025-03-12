////////////////////////////////////////////////////////////////////////////////
// File: matrices.hpp                                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_MATRICES_HPP_
#define RESPOND_DATAOPS_MATRICES_HPP_

#include <ostream>
#include <vector>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

/// @brief Namespace containing all Helper Utility Classes
namespace respond::data_ops {

    using Matrix3d = Eigen::Tensor<double, 3>;

    using TimedMatrix3d = std::map<int, Matrix3d>;

    Matrix3d CreateMatrix3d(const int number_behaviors,
                            const int number_interventions,
                            const int number_demographics);
    void PrintMatrix3d(const Matrix3d &matrix, std::ostream &stream);
    void PrintTimedMatrix3d(const TimedMatrix3d &matrix, std::ostream &stream);

    Matrix3d Matrix3dVectorMinimum(const std::vector<Matrix3d> &matrices);

    Matrix3d Matrix3dVectorMultiplied(const std::vector<Matrix3d> &matrices);

    Matrix3d Matrix3dSummedOverTime(const TimedMatrix3d &matrices);

} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_MATRICES_HPP_
