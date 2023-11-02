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

    /// @brief Class used to Calculate CostList after the Simulation has been Run
    class PostSimulationCalculator {
    public:
        /// @brief Main Constructor to create the cost calculator
        /// @param costLoader Cost Loader Class containing Information to
        /// calculate the Cost
        /// @param utilityLoader Utility Loader Class containing Information to
        /// calculate the Utility
        /// @param history History Class containing Simulation State History
        PostSimulationCalculator(Data::History const &history);

        /// @brief Main function to calculate the Cost
        /// @return Cost Struct filled with calculated Cost
        Data::CostList calculateCosts(Data::ICostLoader const &costLoader) const;

        /// @brief Main function to calculate the Utility
        /// @return Cost Struct filled with calculated Utility
        Data::UtilityList
        calculateUtilities(Data::IUtilityLoader const &utilityLoader) const;

        Data::Matrix3d static provideDiscount(Data::Matrix3d data,
                                              double discountRate, int N,
                                              bool isDiscrete = true,
                                              bool weeklyTimestep = true);

        Data::History getHistory() const { return this->history; }

    private:
        Data::History history;

        /// @brief
        /// @param overdose
        /// @param cost
        /// @return
        Data::Matrix3dOverTime
        multiplyDouble(Data::Matrix3dOverTime const &overdose,
                       double const &cost, bool provideDiscount = false,
                       double discountRate = 0.0) const;

        /// @brief
        /// @param state
        /// @param cost
        /// @return
        Data::Matrix3dOverTime
        multiplyMatrix(Data::Matrix3dOverTime const &state,
                       Data::Matrix3d const &cost, bool provideDiscount = false,
                       double discountRate = 0.0) const;
    };
} // namespace Calculator

#endif