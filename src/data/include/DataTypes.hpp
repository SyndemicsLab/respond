//===-- DataTypes.hpp - DataTypes class definition --------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the DataTypes class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef DATA_DATATYPES_HPP_
#define DATA_DATATYPES_HPP_

#include <Eigen/Eigen>
#include <cassert>
#include <cfenv>
#include <iostream>
#include <stdexcept>
#include <unsupported/Eigen/CXX11/Tensor>
#include <vector>

#define ASSERTM(exp, msg) assert(((void)msg, exp))

namespace Data {

    /// @brief Specification for each dimension in the Matrix3d
    enum Dimension { INTERVENTION = 0, OUD = 1, DEMOGRAPHIC_COMBO = 2 };

    /// @brief Eigen 3d Tensor
    using Matrix3d = Eigen::Tensor<double, 3>;

    inline Matrix3d vecMin(std::vector<Matrix3d> const &matrices) {
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

    inline Matrix3d vecMult(std::vector<Matrix3d> const &matrices) {
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
    class Matrix3dOverTime {
    public:
        Matrix3dOverTime() {}
        Matrix3dOverTime(std::vector<Matrix3d> data);
        Matrix3dOverTime(std::vector<Matrix3d> data,
                         std::vector<int> timestepChanges);

        /// @brief Get the Matrix3d at the Specified Timestep
        /// @param timestep Timestep to retrieve the Matrix3d
        /// @return Matrix3d at Specified Timestep
        Matrix3d &getMatrix3dAtTimestep(int timestep);

        Matrix3d &operator()(int timestep);
        Matrix3d operator()(int timestep) const;

        double &operator()(int timestep, int idx1, int idx2, int idx3);
        double operator()(int timestep, int idx1, int idx2, int idx3) const;

        /// @brief Add the Matrix3d at the specified Timestep
        /// @param datapoint Matrix3d Data
        /// @param timestep Timestep to insert the Matrix3d
        void insert(Matrix3d const &datapoint, int timestep);

        /// @brief Return all the Matrices in order by their timestep
        /// @return Vector of Matrix3ds in timestep order
        std::vector<Matrix3d> getMatrices() const;

        std::map<int, Matrix3d> getMappedData() const { return this->data; }

        Matrix3d sumOverTime() const;

    private:
        std::map<int, Matrix3d> data;
    };

    struct History {
        Matrix3dOverTime stateHistory;
        Matrix3dOverTime overdoseHistory;
        Matrix3dOverTime fatalOverdoseHistory;
        Matrix3dOverTime mortalityHistory;
        Matrix3dOverTime interventionAdmissionHistory;
    };

    /// @brief Struct defining all the History Matrices Across the Duration of
    /// the Simulation
    using History = History;

    struct Cost {
        std::string perspective;
        Matrix3dOverTime healthcareCost;
        Matrix3dOverTime nonFatalOverdoseCost;
        Matrix3dOverTime fatalOverdoseCost;
        Matrix3dOverTime pharmaCost;
        Matrix3dOverTime treatmentCost;
    };

    /// @brief
    using Cost = Cost;

    /// @brief Struct defining Cost Matrices Across the Duration of the
    /// Simulation
    using CostList = std::vector<Cost>;

}; // namespace Data

#endif
