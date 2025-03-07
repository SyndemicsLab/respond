////////////////////////////////////////////////////////////////////////////////
// File: data_types.hpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATATYPES_HPP_
#define RESPOND_DATAOPS_DATATYPES_HPP_

#include <cassert>
#include <cfenv>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

namespace respond::data_ops {

    /// @brief Specification for each dimension in the Matrix3d
    enum Dimension { kIntervention = 0, kOud = 1, kDemographicCombo = 2 };

    /// @brief Eigen 3d Tensor
    using Matrix3d = Eigen::Tensor<double, 3>;

    inline Matrix3d VecMin(std::vector<Matrix3d> const &matrices) {
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

    inline Matrix3d VecMult(std::vector<Matrix3d> const &matrices) {
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

    /// @brief Eigen 3d Tensor maintaining Time Order
    class Matrix4d {
    public:
        Matrix4d() {}
        Matrix4d(std::vector<Matrix3d> data);
        Matrix4d(std::vector<Matrix3d> data, std::vector<int> timestep_changes);

        /// @brief Get the Matrix3d at the Specified Timestep
        /// @param timestep Timestep to retrieve the Matrix3d
        /// @return Matrix3d at Specified Timestep
        Matrix3d &GetMatrix3dAtTimestep(int timestep);

        Matrix3d &operator()(int timestep);
        Matrix3d operator()(int timestep) const;

        double &operator()(int timestep, int idx1, int idx2, int idx3);
        double operator()(int timestep, int idx1, int idx2, int idx3) const;

        /// @brief Add the Matrix3d at the specified Timestep
        /// @param datapoint Matrix3d data_ops
        /// @param timestep Timestep to insert the Matrix3d
        void Insert(Matrix3d const &datapoint, int timestep);

        /// @brief Return all the Matrices in order by their timestep
        /// @return Vector of Matrix3ds in timestep order
        std::vector<Matrix3d> GetMatrices() const;

        std::map<int, Matrix3d> GetMappedData() const { return this->data; }

        Matrix3d SumOverTime() const;

    private:
        std::map<int, Matrix3d> data;
    };

    struct History {
        Matrix4d state_history;
        Matrix4d overdose_history;
        Matrix4d fatal_overdose_history;
        Matrix4d mortality_history;
        Matrix4d intervention_admission_history;
    };

    /// @brief Struct defining all the History Matrices Across the Duration of
    /// the Simulation
    using History = History;

    struct Cost {
        std::string perspective;
        Matrix4d healthcare_cost;
        Matrix4d non_fatal_overdose_cost;
        Matrix4d fatal_overdose_cost;
        Matrix4d pharma_cost;
        Matrix4d treatment_cost;
    };

    /// @brief
    using Cost = Cost;

    /// @brief Struct defining Cost Matrices Across the Duration of the
    /// Simulation
    using CostList = std::vector<Cost>;

    struct Totals {
        std::vector<double> base_costs;
        std::vector<double> disc_costs;
        double base_life_years = 0.0;
        double disc_life_years = 0.0;
        double base_utility = 0.0;
        double disc_utility = 0.0;
    };

    using Totals = Totals;

}; // namespace respond::data_ops

#endif // RESPOND_DATAOPS_DATATYPES_HPP_
