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
        PostSimulationCalculator(data_ops::History const &history);

        /// @brief Main function to calculate the Cost
        /// @return Cost Struct filled with calculated Cost
        data_ops::CostList
        calculateCosts(std::shared_ptr<data_ops::ICostLoader> const costLoader,
                       bool discount = false) const;

        /// @brief Main function to calculate the Utility
        /// @return Cost Struct filled with calculated Utility
        data_ops::Matrix4d calculateUtilities(
            std::shared_ptr<data_ops::IUtilityLoader> const utilityLoader,
            UTILITY_TYPE utilType, bool discount = false) const;

        double calculateLifeYears(bool provideDiscount = false,
                                  double discountRate = 0.0) const;

        double totalAcrossTimeAndDims(data_ops::Matrix4d const data) const;

        data_ops::Matrix3d static provideDiscount(data_ops::Matrix3d data,
                                                  double discountRate, int N,
                                                  bool isDiscrete = true,
                                                  bool weeklyTimestep = true);

        data_ops::History getHistory() const { return this->history; }

    private:
        data_ops::History history;

        /// @brief
        /// @param overdose
        /// @param cost
        /// @return
        data_ops::Matrix4d multiplyDouble(data_ops::Matrix4d const &overdose,
                                          double const &cost,
                                          bool provideDiscount = false,
                                          double discountRate = 0.0) const;

        /// @brief
        /// @param state
        /// @param cost
        /// @return
        data_ops::Matrix4d multiplyMatrix(data_ops::Matrix4d const &state,
                                          data_ops::Matrix3d const &cost,
                                          bool provideDiscount = false,
                                          double discountRate = 0.0) const;
    };
} // namespace Calculator

#endif // RESPONDSIMULATION_MODEL_POSTSIMULATIONCALCULATOR_HPP_