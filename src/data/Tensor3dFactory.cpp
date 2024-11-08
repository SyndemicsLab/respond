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

#include "Tensor3dFactory.hpp"
#include <Eigen/Eigen>

namespace data {

    Tensor3d Tensor3dFactory::Create(const uint16_t x, const uint16_t y,
                                     const uint16_t z) {
        Eigen::array<Eigen::Index, 3> order = {x, y, z};
        Tensor3d empty(order);
        empty.setZero();
        return empty;
    }

} // namespace data
