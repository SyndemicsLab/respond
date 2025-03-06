////////////////////////////////////////////////////////////////////////////////
// File: Matrix3dFactory.cpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respondsimulation/data_ops/Matrix3dFactory.hpp>

namespace Matrixify {

    Matrixify::Matrix3d Matrix3dFactory::Create(const int numOUDStates,
                                                const int numInterventions,
                                                const int numDemographics) {
        Eigen::array<Eigen::Index, 3> order = {0, 0, 0};
        order[Matrixify::OUD] = numOUDStates;
        order[Matrixify::INTERVENTION] = numInterventions;
        order[Matrixify::DEMOGRAPHIC_COMBO] = numDemographics;
        Matrixify::Matrix3d empty(order);
        empty.setZero();
        return empty;
    }

} // namespace Matrixify
