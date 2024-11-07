//===-- Matrix3dFactory.cpp - Matrix3dFactory class definition --*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Matrix3dFactory class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Matrix3dFactory.hpp"

namespace matrixify {

    matrixify::Matrix3d Matrix3dFactory::Create(const int numOUDStates,
                                                const int numInterventions,
                                                const int numDemographics) {
        Eigen::array<Eigen::Index, 3> order = {0, 0, 0};
        order[matrixify::OUD] = numOUDStates;
        order[matrixify::INTERVENTION] = numInterventions;
        order[matrixify::DEMOGRAPHIC_COMBO] = numDemographics;
        matrixify::Matrix3d empty(order);
        empty.setZero();
        return empty;
    }

} // namespace matrixify
