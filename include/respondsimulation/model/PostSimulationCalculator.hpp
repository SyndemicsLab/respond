////////////////////////////////////////////////////////////////////////////////
// File: PostSimulationCalculator.hpp                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPONDSIMULATION_MODEL_POSTSIMULATIONCALCULATOR_HPP_
#define RESPONDSIMULATION_MODEL_POSTSIMULATIONCALCULATOR_HPP_

#include <math.h>

#include <respondsimulation/data_ops/CostLoader.hpp>
#include <respondsimulation/data_ops/DataTypes.hpp>
#include <respondsimulation/data_ops/Matrix3dFactory.hpp>
#include <respondsimulation/data_ops/UtilityLoader.hpp>

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

        double totalAcrossTimeAndDims(Matrixify::Matrix4d const data) const;

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
        Matrixify::Matrix4d multiplyDouble(Matrixify::Matrix4d const &overdose,
                                           double const &cost,
                                           bool provideDiscount = false,
                                           double discountRate = 0.0) const;

        /// @brief
        /// @param state
        /// @param cost
        /// @return
        Matrixify::Matrix4d multiplyMatrix(Matrixify::Matrix4d const &state,
                                           Matrixify::Matrix3d const &cost,
                                           bool provideDiscount = false,
                                           double discountRate = 0.0) const;
    };
} // namespace Calculator

#endif // RESPONDSIMULATION_MODEL_POSTSIMULATIONCALCULATOR_HPP_