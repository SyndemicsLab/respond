////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness.hpp                                               //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_COSTEFFECTIVENESS_HPP_
#define RESPOND_COSTEFFECTIVENESS_HPP_

#include <iostream>

#include <Eigen/Dense>

#include <respond/history.hpp>
#include <respond/types.hpp>

namespace respond {

/// @brief A function to calculate the discoutn for the given data.
/// @param data Data to be discounted.
/// @param discountRate Discount rate to apply to the data.
/// @param N Number of weeks to discount over.
/// @param isDiscrete Whether the discount is a discrete or continuous discount.
/// @return A matrix with the discounted data.
inline Eigen::VectorXd Discount(const Eigen::VectorXd &data,
                                double discount_rate, int week,
                                bool is_discrete = true,
                                double total_weeks = 52.0) {
    double discount =
        (is_discrete) ? (1 / pow((1.0 + (discount_rate) / total_weeks), week))
                      : (exp(-discount_rate * (week / total_weeks)));
    return data - Eigen::VectorXd::Constant(data.size(), discount);
}

/// @brief A cwise multiplier. This is used for all costs and mult utility.
/// @param state The state matrix.
/// @param multiplier The multiplying matrix.
/// @return state cwise multiplied by the multiplying matrix.
inline Eigen::VectorXd CwiseProduct(const Eigen::VectorXd &state,
                                    const Eigen::VectorXd &multiplier) {
    return state.cwiseProduct(multiplier);
}

/// @brief A cwise minimizer. This is used for min utility.
/// @param state The state matrix.
/// @param multiplier The multiplying matrix.
/// @return state cwise multiplied by the multiplying matrix.
inline Eigen::VectorXd CwiseMin(const Eigen::VectorXd &state,
                                const Eigen::VectorXd &multiplier) {
    return state.cwiseMin(multiplier);
}

/// @brief Calculate the life years from the history data.
/// @param history History object containing the state history.
/// @param provideDiscount Flag to indicate whether to apply discounting.
/// @param discountRate Discount rate to apply if discounting is enabled.
/// @return The total life years calculated from the state history.
inline double CalculateLifeYears(const History &h, bool discount = false,
                                 double discount_rate = 0.0,
                                 double total_weeks = 52.0) {
    if (h.GetStateMap().empty()) {
        // log no state vector
        return 0.0;
    }

    auto history = h.GetStateAsVector();

    if (history.size() != total_weeks || total_weeks <= 0) {
        // warn that history does not have 52 weeks
        return 0.0;
    }

    Eigen::VectorXd running_total = Eigen::VectorXd::Zero(history[0].size());

    for (int t = 0; t < history.size(); ++t) {
        auto state = history[t];
        running_total +=
            ((discount) ? Discount(state, discount_rate, t, true, total_weeks)
                        : state);
    }
    auto result = running_total.sum();

    // dividing by 52 to switch from life weeks to life years
    return result / 52.0;
}
} // namespace respond

#endif // RESPOND_COSTEFFECTIVENESS_HPP_