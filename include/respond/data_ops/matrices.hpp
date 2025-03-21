////////////////////////////////////////////////////////////////////////////////
// File: matrices.hpp                                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-21                                                  //
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
#include <unsupported/Eigen/CXX11/Tensor>

/// @brief Namespace containing all Helper Utility Classes
namespace respond::data_ops {

    using Matrix3d = Eigen::Tensor<double, 3>;

    using TimedMatrix3d = std::map<int, Matrix3d>;

    inline Matrix3d CreateMatrix3d(const int x_1, const int x_2,
                                   const int x_3) {
        Eigen::array<Eigen::Index, 3> order = {x_1, x_2, x_3};
        Matrix3d empty(order);
        empty.setZero();
        return empty;
    }

    inline void PrintMatrix3d(const Matrix3d &matrix, std::ostream &stream) {
        Matrix3d::Dimensions dim = matrix.dimensions();
        for (int i = 0; i < dim[2]; ++i) {
            // intervention iterated along columns
            for (int j = 0; j < dim[0]; ++j) {
                // oud state is the row
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

    inline void PrintTimedMatrix3d(const TimedMatrix3d &data,
                                   std::ostream &stream) {
        for (const auto &kv : data) {
            stream << "++++++++ NEW TIMESTEP ++++++++" << std::endl;
            PrintMatrix3d(kv.second, stream);
        }
    }

    inline Matrix3d
    Matrix3dVectorMinimum(const std::vector<Matrix3d> &matrices) {
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

    inline TimedMatrix3d
    MultiplyTimedMatrix3dByDouble(const TimedMatrix3d &data,
                                  const double &value) {
        TimedMatrix3d result;

        for (int t = 0; t < data.size(); t++) {
            auto ref = data.at(t);
            Matrix3d temp = CreateMatrix3d(ref.dimension(0), ref.dimension(1),
                                           ref.dimension(2))
                                .constant(value);
            result[t] = ref * temp;
        }
        return result;
    }

    inline TimedMatrix3d
    MultiplyTimedMatrix3dByMatrix(const TimedMatrix3d &state,
                                  const Matrix3d &value) {
        TimedMatrix3d result;
        for (int t = 0; t < state.size(); ++t) {
            Matrix3d temp = state.at(t) * value;
            result[t] = temp;
        }
        return result;
    }

} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_MATRICES_HPP_
