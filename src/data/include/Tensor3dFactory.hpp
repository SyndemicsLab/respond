//===-- Tensor3dFactory.hpp - Tensor3dFactory class definition --*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Tensor3dFactory class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef UTILITIES_Tensor3dFactory_HPP_
#define UTILITIES_Tensor3dFactory_HPP_

#include "Tensor3d.hpp"
#include <cstdint>

/// @brief Namespace containing all Helper Utility Classes
namespace data {

    /// @brief Factory to create a Matrix3d Object
    class Tensor3dFactory {
    public:
        Tensor3dFactory() {};

        /// @brief Main Static Function used to generate Matrix3d Objects
        /// @param numOUDStates Integer specifying number of OUD States
        /// @param numInterventions Integer specifying number of Interventions
        /// @param numDemographics Integer specifying number of Demographic
        /// Combinations
        /// @return New Matrix3d Object of specified dimensions
        static Tensor3d Create(const uint16_t x, const uint16_t y,
                               const uint16_t z);
    };

} // namespace data

#endif
