//===-- DataTypes.cpp - DataTypes class definition --------------*- C++ -*-===//
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

#include "DataTypes.hpp"

namespace Data {
    Matrix3dOverTime::Matrix3dOverTime(std::vector<Matrix3d> data) {
        for (int i = 0; i < data.size(); i++) {
            this->data.insert({i, data[i]});
        }
    }

    Matrix3dOverTime::Matrix3dOverTime(std::vector<Matrix3d> data,
                                       std::vector<int> timestepChanges) {
        ASSERTM(data.size() == timestepChanges.size(), "Vector Sizes Correct");

        for (int i = 0; i < data.size(); i++) {
            this->data.insert({timestepChanges[i], data[i]});
        }
    }

    Matrix3d Matrix3dOverTime::getMatrix3dAtTimestep(int timestep) {
        int temp = this->data.begin()->first;
        for (auto itr = this->data.begin(); itr != this->data.end(); ++itr) {
            int key = itr->first;
            if (key >= timestep) {
                return this->data[key];
            }
        }
        return this->data.end()->second;
    }

    void Matrix3dOverTime::insert(Matrix3d datapoint, int timestep) {
        this->data.insert({timestep, datapoint});
    }

    std::vector<Matrix3d> Matrix3dOverTime::getMatrices() {
        std::vector<Matrix3d> values;
        for (auto const &x : this->data) {
            Matrix3d mat = x.second;
            values.push_back(mat);
        }
        return values;
    }
} // namespace Data
