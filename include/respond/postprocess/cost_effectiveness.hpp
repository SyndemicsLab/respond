////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness.hpp                                               //
// Project: respond                                                           //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_POSTPROCESS_COSTEFFECTIVENESS_HPP_
#define RESPOND_POSTPROCESS_COSTEFFECTIVENESS_HPP_

#include <Eigen/Dense>

#include <respond/utils/helpers.hpp>
#include <respond/utils/types.hpp>

namespace respond {
namespace postprocess {

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

inline Eigen::VectorXd StampUtilities(const Eigen::VectorXd &state,
                                      const Eigen::VectorXd &utility) {
    if (state.size() != utility.size()) {
        // log error: state and utility size mismatch
        return Eigen::VectorXd::Ones(state.size());
    }
    return state.cwiseProduct(utility);
}

inline Eigen::VectorXd
StampUtilitiesOverTime(const utils::HistoryOverTime &history,
                       const Eigen::VectorXd &utility,
                       utils::UtilityType util_type, bool discount = false,
                       double discount_rate = 0.0) {
    Eigen::VectorXd utilities = Eigen::VectorXd::Ones(utility.size());
    for (const auto &kv : history) {
        auto stamp = StampUtilities(kv.second.state, utility);
        if (discount) {
            stamp = Discount(stamp, discount_rate, kv.first);
        }
        switch (util_type) {
        case utils::UtilityType::kMin:
            utilities = utilities.cwiseMin(stamp);
            break;
        case utils::UtilityType::kMult:
            utilities = utilities.cwiseProduct(stamp);
            break;
        default:
            break;
        }
    }
    return utilities;
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
    utils::CheckVectorLengths(perspectives, healthcare_costs, aod_costs,
                              fod_costs, pharma_costs, treatment_costs);
    utils::CostPerspectives cost_perspectives;
    for (int i = 0; i < perspectives.size(); ++i) {
        cost_perspectives[perspectives[i]] = StampCostsOverTime(
            history_over_time, healthcare_costs[i], aod_costs[i], fod_costs[i],
            pharma_costs[i], treatment_costs[i], discount, discount_rate);
    }
    return cost_perspectives;
}

/// @brief Calculate the life years from the history data.
/// @param history History object containing the state history.
/// @param provideDiscount Flag to indicate whether to apply discounting.
/// @param discountRate Discount rate to apply if discounting is enabled.
/// @return The total life years calculated from the state history.
inline double CalculateLifeYears(const utils::HistoryOverTime &history,
                                 bool discount = false,
                                 double discount_rate = 0.0) {
    if (history.empty()) {
        // log no state vector
        return 0.0;
    }

    if (history.size() != 52) {
        // warn that history does not have 52 weeks
        return 0.0;
    }

    auto running_total =
        Eigen::VectorXd::Zero(history.begin()->second.state.size());

    for (int t = 0; t < history.size(); ++t) {
        auto state = history.at(t).state;
        running_total +=
            ((discount) ? Discount(state, discount_rate, t) : state);
    }
    auto result = running_total.sum();

    // dividing by 52 to switch from life weeks to life years
    return result / 52.0;
}

/// @brief Calculate the total costs from a list of costs.
/// @param cost_list List of costs to calculate total costs from.
/// @return A vector containing the total costs for each cost entry.
inline std::vector<double>
CalculateTotalCosts(const utils::CostsOverTime &costs) {
    std::vector<double> result;
    for (const auto &cost : costs) {
        result.push_back(cost.second.healthcare.sum() +
                         cost.second.non_fatal_overdoses.sum() +
                         cost.second.fatal_overdoses.sum() +
                         cost.second.pharmaceuticals.sum() +
                         cost.second.treatments.sum());
    }
    return result;
}
} // namespace postprocess
} // namespace respond

#endif // RESPOND_POSTPROCESS_COSTEFFECTIVENESS_HPP_