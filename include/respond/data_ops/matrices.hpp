////////////////////////////////////////////////////////////////////////////////
// File: matrices.hpp                                                         //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_MATRICES_HPP_
#define RESPOND_DATAOPS_MATRICES_HPP_

#include <map>
#include <ostream>
#include <vector>

#include <Eigen/Eigen>

#include <respond/data_ops/data_types.hpp>

namespace respond {
namespace data_ops {

/// @brief Function to create an zeroed out 3D Eigen Tensor.
/// @param x_1 Dimension 1 size.
/// @param x_2 Dimension 2 size.
/// @param x_3 Dimension 3 size.
/// @return A zeroed out 3D Eigen Tensor with sizes x_1, x_2, and x_3.
inline Matrix3d CreateMatrix3d(const int x_1, const int x_2, const int x_3) {
    Eigen::array<Eigen::Index, 3> order = {x_1, x_2, x_3};
    Matrix3d empty(order);
    empty.setZero();
    return empty;
}

/// @brief Function to print a 3D Eigen Tensor to the provided stream.
/// @param matrix The 3D Eigen Tensor to print.
/// @param stream The output stream to send the output.
inline void PrintMatrix3d(const Matrix3d &matrix, std::ostream &stream) {
    Matrix3d::Dimensions dim = matrix.dimensions();
    for (int i = 0; i < dim[2]; ++i) {
        for (int j = 0; j < dim[0]; ++j) {
            for (int k = 0; k < dim[1]; ++k) {
                stream << matrix(j, k, i) << " ";
            }
            stream << std::endl;
        }
        if (i != (dim[2] - 1)) {
            stream << "===========================" << std::endl;
        }
    }
}

/// @brief Function to print a 3D Eigent Tensor Map over its keys.
/// @param data map of int to Matrix3d, each int is a timestep.
/// @param stream The output stream to send the output.
inline void PrintTimedMatrix3d(const TimedMatrix3d &data,
                               std::ostream &stream) {
    for (const auto &kv : data) {
        stream << "++++++++ NEW TIMESTEP ++++++++" << std::endl;
        PrintMatrix3d(kv.second, stream);
    }
}

/// @brief Function to find the minimum value amongst a vector of 3D Eigen
/// Tensors.
/// @param matrices A vector of 3D Eigen Tensors.
/// @return The smallest 3D Eigen Tensor from the vector.
inline Matrix3d Matrix3dVectorMinimum(const std::vector<Matrix3d> &matrices) {
    if (matrices.empty()) {
        return {};
    }
    Matrix3d smallest = matrices[0];
    auto dims = smallest.dimensions();
    for (Matrix3d matrix : matrices) {
        if (matrix.dimensions() != dims) {
            // throw error
            return {};
        }
        smallest = smallest.cwiseMin(matrix);
    }
    return smallest;
}

/// @brief Function to multiply a vector of 3D Eigen Tensors together.
/// @param matrices A vector of 3D Eigen Tensors.
/// @return The product of the vector.
inline Matrix3d
Matrix3dVectorMultiplied(const std::vector<Matrix3d> &matrices) {
    if (matrices.empty()) {
        return {};
    }

    auto dims = matrices[0].dimensions();
    Matrix3d mult(dims);
    mult.setConstant(1.0);

    for (Matrix3d matrix : matrices) {
        if (matrix.dimensions() != dims) {
            // throw error
            return {};
        }
        mult *= matrix;
    }
    return mult;
}

/// @brief Function to sum 3D Eigen Tensors across their map keys.
/// @param matrices A map of int to 3D Eigen Tensors, where each int is a
/// timestep.
/// @return The summed 3D Eigen Tensor.
inline Matrix3d TimedMatrix3dSummed(const TimedMatrix3d &matrices) {
    if (matrices.size() <= 0) {
        // log empty data
        return {};
    }

    Matrix3d running_sum(matrices.begin()->second.dimensions());
    for (const auto &kv : matrices) {
        running_sum += kv.second;
    }
    return running_sum;
}

/// @brief Function to sum all values in a TimedMatrix3d.
/// @param data A map of int to 3D Eigen Tensors, where each int is a timestep.
/// @return The sum of all dimensions of all 3D Eigen Tensors in the map.
inline double TimedMatrix3dSummedOverDimensions(const TimedMatrix3d &data) {
    if (data.size() <= 0) {
        return 0.0;
    }

    Matrix3d running_total(data.begin()->second.dimensions());
    running_total = running_total.setZero();
    for (auto kv : data) {
        running_total += kv.second;
    }

    Eigen::Tensor<double, 0> result = running_total.sum();
    return result(0);
}

/// @brief Function to multiply a TimedMatrix3d by a scalar.
/// @param data A map of int to 3D Eigen Tensors, where each int is a timestep.
/// @param value The scalar value to multiply each 3D Eigen Tensor by.
/// @return The resulting TimedMatrix3d after multiplication.
inline TimedMatrix3d MultiplyTimedMatrix3dByDouble(const TimedMatrix3d &data,
                                                   const double &value) {
    TimedMatrix3d result;

    for (int t = 0; t < data.size(); t++) {
        auto ref = data.at(t);
        Matrix3d temp =
            CreateMatrix3d(ref.dimension(0), ref.dimension(1), ref.dimension(2))
                .constant(value);
        result[t] = ref * temp;
    }
    return result;
}

/// @brief Function to multiply a TimedMatrix3d by a 3D Eigen Tensor.
/// @param state A map of int to 3D Eigen Tensors, where each int is a timestep.
/// @param value The 3D Eigen Tensor to multiply against the TimedMatrix3d.
/// @return The resulting TimedMatrix3d after multiplication.
inline TimedMatrix3d MultiplyTimedMatrix3dByMatrix(const TimedMatrix3d &state,
                                                   const Matrix3d &value) {
    TimedMatrix3d result;
    for (int t = 0; t < state.size(); ++t) {
        Matrix3d temp = state.at(t) * value;
        result[t] = temp;
    }
    return result;
}

} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_MATRICES_HPP_
