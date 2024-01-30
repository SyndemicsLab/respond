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

    enum class UTILITY_TYPE { MIN, MULT };

    /// @brief Class used to Calculate CostList after the Simulation has been
    /// Run
    class PostSimulationCalculator {
    public:
        /// @brief Main Constructor to create the cost calculator
        /// @param costLoader Cost Loader Class containing Information to
        /// calculate the Cost
        /// @param utilityLoader Utility Loader Class containing Information to
        /// calculate the Utility
        /// @param history History Class containing Simulation State History
        PostSimulationCalculator(Matrixify::History const &history);

        /// @brief Main function to calculate the Cost
        /// @return Cost Struct filled with calculated Cost
        Matrixify::CostList
        calculateCosts(std::shared_ptr<Matrixify::ICostLoader> const costLoader,
                       bool discount = false) const;

        /// @brief Main function to calculate the Utility
        /// @return Cost Struct filled with calculated Utility
        Matrixify::Matrix4d calculateUtilities(
            std::shared_ptr<Matrixify::IUtilityLoader> const utilityLoader,
            UTILITY_TYPE utilType, bool discount = false) const;

        double calculateLifeYears(bool provideDiscount = false,
                                  double discountRate = 0.0) const;

        double
        totalAcrossTimeAndDims(Matrixify::Matrix4d const data) const;

        Matrixify::Matrix3d static provideDiscount(Matrixify::Matrix3d data,
                                                   double discountRate, int N,
                                                   bool isDiscrete = true,
                                                   bool weeklyTimestep = true);

        Matrixify::History getHistory() const { return this->history; }

    private:
        Matrixify::History history;

        /// @brief
        /// @param overdose
        /// @param cost
        /// @return
        Matrixify::Matrix4d
        multiplyDouble(Matrixify::Matrix4d const &overdose,
                       double const &cost, bool provideDiscount = false,
                       double discountRate = 0.0) const;

        /// @brief
        /// @param state
        /// @param cost
        /// @return
        Matrixify::Matrix4d
        multiplyMatrix(Matrixify::Matrix4d const &state,
                       Matrixify::Matrix3d const &cost,
                       bool provideDiscount = false,
                       double discountRate = 0.0) const;
    };
} // namespace Calculator

#endif