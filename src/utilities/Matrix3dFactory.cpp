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

namespace Utilities {

    Data::Matrix3d Matrix3dFactory::Create(int numOUDStates,
                                           int numInterventions,
                                           int numDemographics) {
        Eigen::array<Eigen::Index, 3> order = {0, 0, 0};
        // std::array<long int, 3> order = {0, 0, 0};
        order[Data::OUD] = numOUDStates;
        order[Data::INTERVENTION] = numInterventions;
        order[Data::DEMOGRAPHIC_COMBO] = numDemographics;
        Data::Matrix3d empty(order);
        empty.setZero();
        return empty;
    }

} // namespace Utilities
