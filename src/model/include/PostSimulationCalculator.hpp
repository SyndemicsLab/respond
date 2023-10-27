//===-- CostCalculator.hpp - CostCalculator class definition ----*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the CostCalculator class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef MODEL_COSTCALCULATOR_HPP_
#define MODEL_COSTCALCULATOR_HPP_

#include <math.h>

#include "CostLoader.hpp"
#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include "UtilityLoader.hpp"

/// @brief Namespace for all Post Simulation Calculations
namespace Calculator {

    /// @brief Class used to Calculate Costs after the Simulation has been Run
    class PostSimulationCalculator {
    public:
        /// @brief Main Constructor to create the cost calculator
        /// @param costLoader Cost Loader Class containing Information to
        /// calculate the Cost
        /// @param utilityLoader Utility Loader Class containing Information to
        /// calculate the Utility
        /// @param history History Class containing Simulation State History
        PostSimulationCalculator(Data::CostLoader const &costLoader,
                                 Data::UtilityLoader const &utilityLoader,
                                 Data::History const &history);

        /// @brief Main function to calculate the Cost
        /// @return Cost Struct filled with calculated Cost
        Data::Costs calculateCosts() const;

        /// @brief Main function to calculate the Utility
        /// @return Cost Struct filled with calculated Utility
        Data::Utility calculateUtility() const;

        Data::Matrix3d static provideDiscount(Data::Matrix3d data,
                                              double discountRate, int N);

    private:
        Data::History history;
        Data::CostLoader costLoader;
        Data::UtilityLoader utilityLoader;

        /// @brief
        /// @param overdose
        /// @param cost
        /// @return
        Data::Matrix3dOverTime
        multiplyDouble(Data::Matrix3dOverTime const &overdose,
                       double const &cost) const;

        /// @brief
        /// @param state
        /// @param cost
        /// @return
        Data::Matrix3dOverTime
        multiplyMatrix(Data::Matrix3dOverTime const &state,
                       Data::Matrix3d const &cost) const;
    };
} // namespace Calculator

#endif