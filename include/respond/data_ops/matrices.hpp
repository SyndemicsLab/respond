////////////////////////////////////////////////////////////////////////////////
// File: matrices.hpp                                                         //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
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

/// @brief
/// @param x_1
/// @param x_2
/// @param x_3
/// @return
inline Matrix3d CreateMatrix3d(const int x_1, const int x_2, const int x_3) {
    Eigen::array<Eigen::Index, 3> order = {x_1, x_2, x_3};
    Matrix3d empty(order);
    empty.setZero();
    return empty;
}

/// @brief
/// @param matrix
/// @param stream
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

/// @brief
/// @param data
/// @param stream
inline void PrintTimedMatrix3d(const TimedMatrix3d &data,
                               std::ostream &stream) {
    for (const auto &kv : data) {
        stream << "++++++++ NEW TIMESTEP ++++++++" << std::endl;
        PrintMatrix3d(kv.second, stream);
    }
}

/// @brief
/// @param matrices
/// @return
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

/// @brief
/// @param matrices
/// @return
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

/// @brief
/// @param matrices
/// @return
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

/// @brief
/// @param data
/// @return
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

/// @brief
/// @param data
/// @param value
/// @return
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

/// @brief
/// @param state
/// @param value
/// @return
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
