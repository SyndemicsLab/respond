////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness.hpp                                               //
// Project: respond                                                           //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_POSTPROCESS_COSTEFFECTIVENESS_HPP_
#define RESPOND_POSTPROCESS_COSTEFFECTIVENESS_HPP_

#include <Eigen/Dense>

#include <respond/utils/data_types.hpp>

namespace respond {
namespace postprocess {

template <class... Args>
inline constexpr bool CheckVectorLengths(Args const &...args) {
    if constexpr (sizeof...(Args) == 0) {
        return true;
    } else {
        return [](auto const &first, auto const &...rest) {
            return ((first.size() == rest.size()) && ...);
        }(args...);
    }
}

/// @brief A function to calculate the discoutn for the given data.
/// @param data Data to be discounted.
/// @param discountRate Discount rate to apply to the data.
/// @param N Number of weeks to discount over.
/// @param isDiscrete Whether the discount is a discrete or continuous discount.
/// @return A matrix with the discounted data.
inline Eigen::VectorXd Discount(const Eigen::VectorXd &data,
                                double discount_rate, int week,
                                bool is_discrete = true) {
    double discount = (is_discrete)
                          ? (1 / pow((1.0 + (discount_rate) / 52.0), week))
                          : (exp(-discount_rate * (week / 52)));

    return data - Eigen::VectorXd::Constant(data.size(), discount);
}

inline void DiscountCostStamp(utils::CostStamp &cost_stamp,
                              double discount_rate, int week,
                              bool is_discrete = true) {
    cost_stamp.healthcare =
        Discount(cost_stamp.healthcare, discount_rate, week, is_discrete);
    cost_stamp.non_fatal_overdoses = Discount(cost_stamp.non_fatal_overdoses,
                                              discount_rate, week, is_discrete);
    cost_stamp.fatal_overdoses =
        Discount(cost_stamp.fatal_overdoses, discount_rate, week, is_discrete);
    cost_stamp.pharmaceuticals =
        Discount(cost_stamp.pharmaceuticals, discount_rate, week, is_discrete);
    cost_stamp.treatments =
        Discount(cost_stamp.treatments, discount_rate, week, is_discrete);
}

inline utils::CostStamp StampCosts(const Eigen::VectorXd &state,
                                   const Eigen::VectorXd &healthcare_costs,
                                   const Eigen::VectorXd &aod_costs,
                                   const Eigen::VectorXd &fod_costs,
                                   const Eigen::VectorXd &pharma_costs,
                                   const Eigen::VectorXd &treatment_costs) {
    utils::CostStamp cost_stamp;
    cost_stamp.healthcare = state * healthcare_costs;
    cost_stamp.non_fatal_overdoses = state * aod_costs;
    cost_stamp.fatal_overdoses = state * fod_costs;
    cost_stamp.pharmaceuticals = state * pharma_costs;
    cost_stamp.treatments = state * treatment_costs;
    return cost_stamp;
}

inline utils::CostsOverTime StampCostsOverTime(
    const utils::HistoryOverTime &history_over_time,
    const Eigen::VectorXd &healthcare_costs, const Eigen::VectorXd &aod_costs,
    const Eigen::VectorXd &fod_costs, const Eigen::VectorXd &pharma_costs,
    const Eigen::VectorXd &treatment_costs, bool discount = false,
    double discount_rate = 0.0) {
    utils::CostsOverTime costs_over_time;
    for (const auto &kv : history_over_time) {
        costs_over_time[kv.first] =
            StampCosts(kv.second.state, healthcare_costs, aod_costs, fod_costs,
                       pharma_costs, treatment_costs);
        if (discount) {
            DiscountCostStamp(costs_over_time[kv.first], discount_rate,
                              kv.first);
        }
    }
    return costs_over_time;
}

inline utils::CostPerspectives
CalculatePerspectives(const utils::HistoryOverTime &history_over_time,
                      const std::vector<std::string> &perspectives,
                      const std::vector<Eigen::VectorXd> &healthcare_costs,
                      const std::vector<Eigen::VectorXd> &aod_costs,
                      const std::vector<Eigen::VectorXd> &fod_costs,
                      const std::vector<Eigen::VectorXd> &pharma_costs,
                      const std::vector<Eigen::VectorXd> &treatment_costs,
                      bool discount = false, double discount_rate = 0.0) {
    CheckVectorLengths(perspectives, healthcare_costs, aod_costs, fod_costs,
                       pharma_costs, treatment_costs);
    utils::CostPerspectives cost_perspectives;
    for (int i = 0; i < perspectives.size(); ++i) {
        cost_perspectives[perspectives[i]] = StampCostsOverTime(
            history_over_time, healthcare_costs[i], aod_costs[i], fod_costs[i],
            pharma_costs[i], treatment_costs[i], discount, discount_rate);
    }
    return cost_perspectives;
}

/// @brief Calculate the utilities for the given history and utility data.
/// @param history History to calculate utilities over.
/// @param utility_loader UtilityLoader containing the utility data.
/// @param util_type Type of utility calculation to perform.
/// @param discount Flag to indicate whether to apply discounting.
/// @param discount_rate Discount rate to apply if discounting is enabled.
/// @return The calculated utilities as a TimedMatrix3d object.
inline preprocess::TimedMatrix3d
CalculateUtilities(const preprocess::History &history,
                   const preprocess::UtilityLoader &utility_loader,
                   preprocess::UtilityType util_type, bool discount = false,
                   double discount_rate = 0.0) {
    std::vector<preprocess::Matrix3d> utilityMatrices = {
        utility_loader.GetBackgroundUtility("utility"),
        utility_loader.GetOUDUtility("utility"),
        utility_loader.GetSettingUtility("utility")};

    preprocess::Matrix3d util;
    switch (util_type) {
    case preprocess::UtilityType::kMin:
        util = preprocess::Matrix3dVectorMinimum(utilityMatrices);
        break;
    case preprocess::UtilityType::kMult:
        util = preprocess::Matrix3dVectorMultiplied(utilityMatrices);
        break;
    default:
        break;
    }

    preprocess::TimedMatrix3d utilities =
        preprocess::MultiplyTimedMatrix3dByMatrix(history.state_history, util);

    if (!discount) {
        return utilities;
    }

    for (auto kv : utilities) {
        kv.second = CalculateDiscount(kv.second, discount_rate, kv.first);
    }
    return utilities;
}

/// @brief Calculate the life years from the history data.
/// @param history History object containing the state history.
/// @param provideDiscount Flag to indicate whether to apply discounting.
/// @param discountRate Discount rate to apply if discounting is enabled.
/// @return The total life years calculated from the state history.
inline double CalculateLifeYears(const preprocess::History &history,
                                 bool provideDiscount = false,
                                 double discountRate = 0.0) {
    if (history.state_history.size() <= 0) {
        // log no state vector
        return 0.0;
    }

    preprocess::Matrix3d running_total(
        history.state_history.begin()->second.dimensions());
    running_total = running_total.setZero();

    for (int t = 0; t < history.state_history.size(); ++t) {
        running_total +=
            ((provideDiscount) ? CalculateDiscount(history.state_history.at(t),
                                                   discountRate, t)
                               : history.state_history.at(t));
    }
    Eigen::Tensor<double, 0> result = running_total.sum();

    // dividing by 52 to switch from life weeks to life years
    return result(0) / 52.0;
}

/// @brief Calculate the total costs from a list of costs.
/// @param cost_list List of costs to calculate total costs from.
/// @return A vector containing the total costs for each cost entry.
inline std::vector<double>
CalculateTotalCosts(const preprocess::CostList &cost_list) {
    std::vector<double> result;
    for (preprocess::Cost cost : cost_list) {
        double totalCost =
            preprocess::TimedMatrix3dSummedOverDimensions(
                cost.healthcare_cost) +
            preprocess::TimedMatrix3dSummedOverDimensions(
                cost.fatal_overdose_cost) +
            preprocess::TimedMatrix3dSummedOverDimensions(
                cost.non_fatal_overdose_cost) +
            preprocess::TimedMatrix3dSummedOverDimensions(cost.pharma_cost) +
            preprocess::TimedMatrix3dSummedOverDimensions(cost.treatment_cost);
        result.push_back(totalCost);
    }
    return result;
}
} // namespace postprocess
} // namespace respond

#endif // RESPOND_POSTPROCESS_COSTEFFECTIVENESS_HPP_