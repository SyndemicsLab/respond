////////////////////////////////////////////////////////////////////////////////
// File: DataTypes.cpp                                                        //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/DataTypes.hpp>

namespace data_ops {
    Matrix4d::Matrix4d(std::vector<Matrix3d> data) {
        for (int i = 0; i < data.size(); i++) {
            this->data.insert({i, data[i]});
        }
    }

    Matrix4d::Matrix4d(std::vector<Matrix3d> data,
                       std::vector<int> timestepChanges) {
        ASSERTM(data.size() == timestepChanges.size(), "Vector Sizes Correct");

        for (int i = 0; i < data.size(); i++) {
            this->data.insert({timestepChanges[i], data[i]});
        }
    }

    Matrix3d &Matrix4d::getMatrix3dAtTimestep(int timestep) {
        if (this->data.find(timestep) == this->data.end()) {
            // std::cout << this->data.end()->first << std::endl;
            // return this->data.end()->second;
            throw std::invalid_argument("The Provided Timestep " +
                                        std::to_string(timestep) +
                                        " Does Not Exist");
        } else {
            return this->data.at(timestep);
        }
    }

    void Matrix4d::insert(Matrix3d const &datapoint, int timestep) {
        this->data[timestep] = datapoint;
    }

    std::vector<Matrix3d> Matrix4d::getMatrices() const {
        std::vector<Matrix3d> values;
        for (auto const &x : this->data) {
            Matrix3d mat = x.second;
            values.push_back(mat);
        }
        return values;
    }

    Matrix3d &Matrix4d::operator()(int timestep) {
        return this->getMatrix3dAtTimestep(timestep);
    }

    Matrix3d Matrix4d::operator()(int timestep) const {
        if (this->data.find(timestep) == this->data.end()) {
            throw std::invalid_argument("The Provided Timestep " +
                                        std::to_string(timestep) +
                                        " Does Not Exist");
        } else {
            Matrix3d temp = this->data.at(timestep);
            return temp;
        }
    }

    double &Matrix4d::operator()(int timestep, int idx1, int idx2, int idx3) {
        return this->getMatrix3dAtTimestep(timestep)(idx1, idx2, idx3);
    }

    double Matrix4d::operator()(int timestep, int idx1, int idx2,
                                int idx3) const {
        if (this->data.find(timestep) == this->data.end()) {
            throw std::invalid_argument("The Provided Timestep " +
                                        std::to_string(timestep) +
                                        " Does Not Exist");
        } else {
            Matrix3d temp = this->data.at(timestep);
            return temp(idx1, idx2, idx3);
        }
    }

    Matrix3d Matrix4d::sumOverTime() const {
        std::vector<Matrix3d> matrices = this->getMatrices();
        if (matrices.size() <= 0) {
            // log empty data
            return {};
        }

        Matrix3d runningSum(matrices[0].dimensions());

        for (Matrix3d matrix : matrices) {
            runningSum += matrix;
        }
        return runningSum;
    }

} // namespace data_ops
