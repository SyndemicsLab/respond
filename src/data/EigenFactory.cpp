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

#include <Eigen/Eigen>
#include <synmodels/data/EigenFactory.hpp>

namespace synmodels::data {

    Eigen::MatrixXd EigenFactory::CreateMatrix(const uint16_t x,
                                               const uint16_t y) {
        Eigen::MatrixXd empty(x, y);
        empty.setZero();
        return empty;
    }

    Eigen::VectorXd EigenFactory::CreateVector(const uint16_t N) {
        Eigen::VectorXd empty(N);
        empty.setZero();
        return empty;
    }

} // namespace synmodels::data
