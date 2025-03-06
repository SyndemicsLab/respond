////////////////////////////////////////////////////////////////////////////////
// File: DataFormatter.hpp                                                    //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_DATAFORMATTER_HPP_
#define DATA_DATAFORMATTER_HPP_

#include <respondsimulation/data_ops/DataTypes.hpp>
#include <respondsimulation/data_ops/Matrix3dFactory.hpp>

#include <vector>

namespace data_ops {
    class DataFormatter {
    public:
        DataFormatter() {};
        ~DataFormatter() {};
        void extractTimesteps(std::vector<int> timesteps,
                              data_ops::History &history,
                              data_ops::CostList &costs,
                              data_ops::Matrix4d &utilities, bool costSwitch);

    private:
        data_ops::Matrix4d trimMatrix4d(std::vector<int> timesteps,
                                        data_ops::Matrix4d matrix);

        data_ops::Matrix4d trimAndAddMatrix4d(std::vector<int> timesteps,
                                              data_ops::Matrix4d matrix);
    };
} // namespace data_ops

#endif