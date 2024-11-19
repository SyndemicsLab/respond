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

#include "MatrixXdFactory.hpp"
#include <Eigen/Eigen>

namespace data {

    Eigen::MatrixXd MatrixXdFactory::Create(const uint16_t x,
                                            const uint16_t y) {
        Eigen::array<Eigen::Index, 2> order = {x, y};
        Eigen::MatrixXd empty(order);
        empty.setZero();
        return empty;
    }

} // namespace data
