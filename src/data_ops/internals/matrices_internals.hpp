////////////////////////////////////////////////////////////////////////////////
// File: matrices_internals.hpp                                               //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-12                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_MATRICESINTERNALS_HPP_
#define RESPOND_DATAOPS_MATRICESINTERNALS_HPP_

#include <respond/data_ops/matrices.hpp>

#include <respond/data_ops/data_types.hpp>
#include <respond/utils/logging.hpp>

namespace respond::data_ops {

    Matrix3d Matrix3dVectorMinimum(const std::vector<Matrix3d> &matrices) {
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

    Matrix3d Matrix3dVectorMultiplied(const std::vector<Matrix3d> &matrices) {
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

    Matrix3d CreateMatrix3d(const int number_behaviors,
                            const int number_interventions,
                            const int number_demographics) {
        {
            Eigen::array<Eigen::Index, 3> order = {0, 0, 0};
            order[Dimension::kOud] = number_behaviors;
            order[Dimension::kIntervention] = number_interventions;
            order[Dimension::kDemographicCombo] = number_demographics;
            Matrix3d empty(order);
            empty.setZero();
            return empty;
        }
    }

    Matrix3d Matrix3dSummedOverTime(const TimedMatrix3d &matrices) {
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

    void PrintMatrix3d(const Matrix3d &matrix, std::ostream &stream) {
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

    void PrintTimedMatrix3d(const TimedMatrix3d &matrices,
                            std::ostream &stream) {
        for (const auto &kv : matrices) {
            stream << "++++++++ NEW TIMESTEP ++++++++" << std::endl;
            PrintMatrix3d(kv.second, stream);
        }
    }

} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_MATRICESINTERNALS_HPP_