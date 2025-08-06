////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness.hpp                                               //
// Project: respond                                                           //
// Created Date: 2025-08-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_COSTEFFECTIVENESS_HPP_
#define RESPOND_COSTEFFECTIVENESS_HPP_

#include <iostream>

#include <Eigen/Dense>

#include <respond/helpers.hpp>
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

/// @brief A function used to apply the discount to a cost stamp.
/// @param cost_stamp The cost stamp to apply a discount to.
/// @param discount_rate The discount rate to apply to the cost stamp.
/// @param week The week of the simulation this cost stamp applies to.
/// @param is_discrete Whether or not the discount should be discrete.
/// @param total_weeks How many total weeks were run through the simulation.
inline void DiscountCostStamp(CostStamp &cost_stamp, double discount_rate,
                              int week, bool is_discrete = true,
                              double total_weeks = 52.0) {
    cost_stamp.healthcare = Discount(cost_stamp.healthcare, discount_rate, week,
                                     is_discrete, total_weeks);
    cost_stamp.non_fatal_overdoses =
        Discount(cost_stamp.non_fatal_overdoses, discount_rate, week,
                 is_discrete, total_weeks);
    cost_stamp.fatal_overdoses =
        Discount(cost_stamp.fatal_overdoses, discount_rate, week, is_discrete,
                 total_weeks);
    cost_stamp.pharmaceuticals =
        Discount(cost_stamp.pharmaceuticals, discount_rate, week, is_discrete,
                 total_weeks);
    cost_stamp.treatments = Discount(cost_stamp.treatments, discount_rate, week,
                                     is_discrete, total_weeks);
}

/// @brief A function to stamp the costs at a given state.
/// @param state The state at which to capture the costs.
/// @param healthcare_costs The healthcare costs to apply.
/// @param aod_costs The all overdose costs to apply.
/// @param fod_costs The fatal overdose costs to apply.
/// @param pharma_costs The pharmaceutical costs to apply.
/// @param treatment_costs The treatment costs to apply.
/// @return A new cost stamp given the costs and state.
inline CostStamp StampCosts(const Eigen::VectorXd &state,
                            const Eigen::VectorXd &healthcare_costs,
                            const Eigen::VectorXd &aod_costs,
                            const Eigen::VectorXd &fod_costs,
                            const Eigen::VectorXd &pharma_costs,
                            const Eigen::VectorXd &treatment_costs) {
    if (!CheckVectorLengths(state, healthcare_costs, aod_costs, fod_costs,
                            pharma_costs, treatment_costs)) {
        // log error: state and cost vectors size mismatch
        return CostStamp{};
    }
    CostStamp cost_stamp;
    cost_stamp.healthcare = state.cwiseProduct(healthcare_costs);
    cost_stamp.non_fatal_overdoses = state.cwiseProduct(aod_costs);
    cost_stamp.fatal_overdoses = state.cwiseProduct(fod_costs);
    cost_stamp.pharmaceuticals = state.cwiseProduct(pharma_costs);
    cost_stamp.treatments = state.cwiseProduct(treatment_costs);
    return cost_stamp;
}

/// @brief A function to stamp the utilities at a given time.
/// @param state The state at which to capture the utilities.
/// @param utility The utility value to capture.
/// @return A new vector containing the utility given the state and utility.
inline Eigen::VectorXd StampUtilities(const Eigen::VectorXd &state,
                                      const Eigen::VectorXd &utility) {
    if (state.size() != utility.size()) {
        // log error: state and utility size mismatch
        return Eigen::VectorXd::Ones(state.size());
    }
    return state.cwiseProduct(utility);
}

/// @brief A function to stamp the costs over a period of time.
/// @param history_over_time The state history of the model.
/// @param healthcare_costs The healthcare costs to apply to the states.
/// @param aod_costs The all overdoses costs to apply to the states.
/// @param fod_costs The fatal overdose costs to apply to the states.
/// @param pharma_costs The pharmaceutical costs to apply to the states.
/// @param treatment_costs The treatment costs to apply to the states.
/// @param discount Whether or not to apply a discount to each stamp.
/// @param discount_rate The rate of discount to apply.
/// @param total_weeks The total number of weeks in the simulation.
/// @return The Cost Stamps over the model history.
inline CostsOverTime StampCostsOverTime(
    const HistoryOverTime &history_over_time,
    const Eigen::VectorXd &healthcare_costs, const Eigen::VectorXd &aod_costs,
    const Eigen::VectorXd &fod_costs, const Eigen::VectorXd &pharma_costs,
    const Eigen::VectorXd &treatment_costs, bool discount = false,
    double discount_rate = 0.0, double total_weeks = 52.0) {
    CostsOverTime costs_over_time;
    for (const auto &kv : history_over_time) {
        costs_over_time[kv.first] =
            StampCosts(kv.second.state, healthcare_costs, aod_costs, fod_costs,
                       pharma_costs, treatment_costs);
        if (discount) {
            DiscountCostStamp(costs_over_time[kv.first], discount_rate,
                              kv.first, true, total_weeks);
        }
    }
    return costs_over_time;
}

/// @brief A function to stamp the utilities over a period of time.
/// @param history The state history of the model.
/// @param utility The utility value to capture.
/// @param util_type Either Multiplicative or Minimum.
/// @param discount Whether or not to apply a discount to each stamp.
/// @param discount_rate The rate of discount to apply.
/// @param total_weeks The total number of weeks in the simulation.
/// @return The total utility of the history.
inline Eigen::VectorXd StampUtilitiesOverTime(const HistoryOverTime &history,
                                              const Eigen::VectorXd &utility,
                                              UtilityType util_type,
                                              bool discount = false,
                                              double discount_rate = 0.0,
                                              double total_weeks = 52.0) {
    Eigen::VectorXd utilities;
    bool is_first_iteration = true;
    for (const auto &kv : history) {
        auto stamp = StampUtilities(kv.second.state, utility);
        if (discount) {
            stamp = Discount(stamp, discount_rate, kv.first);
        }
        // If this is the first entry in the map, set the utilities vector
        if (is_first_iteration) {
            utilities = stamp;
            is_first_iteration = false;
        }

        switch (util_type) {
        case UtilityType::kMin:
            utilities = utilities.cwiseMin(stamp);
            break;
        case UtilityType::kMult:
            utilities = utilities.cwiseProduct(stamp);
            break;
        default:
            break;
        }
    }
    return utilities;
}

/// @brief A function to calculate costs over history by the given perspective.
/// @param history_over_time The state history of the model.
/// @param perspectives The cost perspective to consider.
/// @param healthcare_costs The healthcare costs to apply to the states.
/// @param aod_costs The all overdoses costs to apply to the states.
/// @param fod_costs The fatal overdose costs to apply to the states.
/// @param pharma_costs The pharmaceutical costs to apply to the states.
/// @param treatment_costs The treatment costs to apply to the states.
/// @param discount Whether or not to apply a discount to each stamp.
/// @param discount_rate The rate of discount to apply.
/// @param total_weeks The total number of weeks in the simulation.
/// @return Cost stamps over time per perspective.
inline CostPerspectives
CalculatePerspectives(const HistoryOverTime &history_over_time,
                      const std::vector<std::string> &perspectives,
                      const std::vector<Eigen::VectorXd> &healthcare_costs,
                      const std::vector<Eigen::VectorXd> &aod_costs,
                      const std::vector<Eigen::VectorXd> &fod_costs,
                      const std::vector<Eigen::VectorXd> &pharma_costs,
                      const std::vector<Eigen::VectorXd> &treatment_costs,
                      bool discount = false, double discount_rate = 0.0,
                      double total_weeks = 52.0) {
    CheckVectorLengths(perspectives, healthcare_costs, aod_costs, fod_costs,
                       pharma_costs, treatment_costs);
    CostPerspectives cost_perspectives;
    for (int i = 0; i < perspectives.size(); ++i) {
        cost_perspectives[perspectives[i]] = StampCostsOverTime(
            history_over_time, healthcare_costs[i], aod_costs[i], fod_costs[i],
            pharma_costs[i], treatment_costs[i], discount, discount_rate,
            total_weeks);
    }
    return cost_perspectives;
}

/// @brief Calculate the life years from the history data.
/// @param history History object containing the state history.
/// @param provideDiscount Flag to indicate whether to apply discounting.
/// @param discountRate Discount rate to apply if discounting is enabled.
/// @return The total life years calculated from the state history.
inline double CalculateLifeYears(const HistoryOverTime &history,
                                 bool discount = false,
                                 double discount_rate = 0.0,
                                 double total_weeks = 52.0) {
    if (history.empty()) {
        // log no state vector
        return 0.0;
    }

    if (history.size() != total_weeks) {
        // warn that history does not have 52 weeks
        return 0.0;
    }

    Eigen::VectorXd running_total =
        Eigen::VectorXd::Zero(history.begin()->second.state.size());

    for (int t = 0; t < history.size(); ++t) {
        auto state = history.at(t).state;
        running_total +=
            ((discount) ? Discount(state, discount_rate, t, true, total_weeks)
                        : state);
    }
    auto result = running_total.sum();

    // dividing by 52 to switch from life weeks to life years
    return result / 52.0;
}

/// @brief Calculate the total costs from a list of costs.
/// @param cost_list List of costs to calculate total costs from.
/// @return A vector containing the total costs for each cost entry.
inline std::vector<double> CalculateTotalCosts(const CostsOverTime &costs) {
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
} // namespace respond

#endif // RESPOND_COSTEFFECTIVENESS_HPP_