////////////////////////////////////////////////////////////////////////////////
// File: post_sim.hpp                                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MODEL_POSTSIM_HPP_
#define RESPOND_MODEL_POSTSIM_HPP_

#include <respond/data_ops/cost_loader.hpp>
#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>
#include <respond/data_ops/utility_loader.hpp>

/// @brief Namespace for all Post Simulation Calculations
namespace respond::model {
    inline respond::data_ops::CostList
    CalculateCosts(const respond::data_ops::History &history,
                   const respond::data_ops::CostLoader &cost_loader,
                   const std::vector<std::string> &perspectives,
                   bool discount = false, double discount_rate = 0.0) {
        data_ops::CostList costs;

        for (std::string perspective : perspectives) {
            data_ops::Cost cost;
            cost.perspective = perspective;
            cost.healthcare_cost =
                respond::data_ops::MultiplyTimedMatrix3dByMatrix(
                    history.state_history,
                    cost_loader.GetHealthcareUtilizationCost(perspective));
            cost.pharma_cost = respond::data_ops::MultiplyTimedMatrix3dByMatrix(
                history.state_history,
                cost_loader.GetPharmaceuticalCost(perspective));
            cost.treatment_cost =
                respond::data_ops::MultiplyTimedMatrix3dByMatrix(
                    history.state_history,
                    cost_loader.GetTreatmentUtilizationCost(perspective));
            cost.non_fatal_overdose_cost =
                respond::data_ops::MultiplyTimedMatrix3dByDouble(
                    history.overdose_history,
                    cost_loader.GetNonFatalOverdoseCost(perspective));
            cost.fatal_overdose_cost =
                respond::data_ops::MultiplyTimedMatrix3dByDouble(
                    history.overdose_history,
                    cost_loader.GetFatalOverdoseCost(perspective));

            if (discount) {
                for (int i = 0; i < history.state_history.size(); ++i) {
                    cost.healthcare_cost[i] = CalculateDiscount(
                        cost.healthcare_cost[i], discount_rate, i);
                    cost.pharma_cost[i] = CalculateDiscount(cost.pharma_cost[i],
                                                            discount_rate, i);
                    cost.treatment_cost[i] = CalculateDiscount(
                        cost.treatment_cost[i], discount_rate, i);
                    cost.non_fatal_overdose_cost[i] = CalculateDiscount(
                        cost.non_fatal_overdose_cost[i], discount_rate, i);
                    cost.fatal_overdose_cost[i] = CalculateDiscount(
                        cost.fatal_overdose_cost[i], discount_rate, i);
                }
            }
            costs.push_back(cost);
        }
        return costs;
    }

    inline respond::data_ops::TimedMatrix3d
    CalculateUtilities(const respond::data_ops::History &history,
                       const respond::data_ops::UtilityLoader &utility_loader,
                       respond::data_ops::UtilityType util_type,
                       bool discount = false, double discount_rate = 0.0) {
        std::vector<data_ops::Matrix3d> utilityMatrices = {
            utility_loader.GetBackgroundUtility("utility"),
            utility_loader.GetOUDUtility("utility"),
            utility_loader.GetSettingUtility("utility")};

        data_ops::Matrix3d util;
        switch (util_type) {
        case respond::data_ops::UtilityType::kMin:
            util = respond::data_ops::Matrix3dVectorMinimum(utilityMatrices);
            break;
        case respond::data_ops::UtilityType::kMult:
            util = respond::data_ops::Matrix3dVectorMultiplied(utilityMatrices);
            break;
        default:
            break;
        }

        respond::data_ops::TimedMatrix3d utilities =
            respond::data_ops::MultiplyTimedMatrix3dByMatrix(
                history.state_history, util);

        if (!discount) {
            return utilities;
        }

        for (auto kv : utilities) {
            kv.second = CalculateDiscount(kv.second, discount_rate, kv.first);
        }
        return utilities;
    }

    inline double CalculateLifeYears(const respond::data_ops::History &history,
                                     bool provideDiscount = false,
                                     double discountRate = 0.0) {
        if (history.state_history.size() <= 0) {
            // log no state vector
            return 0.0;
        }

        respond::data_ops::Matrix3d running_total(
            history.state_history.begin()->second.dimensions());
        running_total = running_total.setZero();

        for (int t = 0; t < history.state_history.size(); ++t) {
            running_total +=
                ((provideDiscount)
                     ? CalculateDiscount(history.state_history.at(t),
                                         discountRate, t)
                     : history.state_history.at(t));
        }
        Eigen::Tensor<double, 0> result = running_total.sum();

        // dividing by 52 to switch from life weeks to life years
        return result(0) / 52.0;
    }

    inline respond::data_ops::Matrix3d
    CalculateDiscount(const respond::data_ops::Matrix3d &data,
                      double discountRate, int N, bool isDiscrete = true) {
        double discountConstant =
            (isDiscrete) ? (1 / pow((1.0 + (discountRate) / 52.0), N))
                         : (exp(-discountRate * (N / 52)));

        data_ops::Matrix3d discount =
            respond::data_ops::CreateMatrix3d(
                data.dimension((int)respond::data_ops::Dimension::kOud),
                data.dimension(
                    (int)respond::data_ops::Dimension::kIntervention),
                data.dimension(
                    (int)respond::data_ops::Dimension::kDemographicCombo))
                .setConstant(discountConstant);
        data_ops::Matrix3d result = data - discount;
        return result;
    }

    inline std::vector<double>
    CalculateTotalCosts(const respond::data_ops::CostList &cost_list) {
        std::vector<double> result;
        for (respond::data_ops::Cost cost : cost_list) {
            double totalCost =
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.healthcare_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.fatal_overdose_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.non_fatal_overdose_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.pharma_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.treatment_cost);
            result.push_back(totalCost);
        }
        return result;
    }
} // namespace respond::model

#endif // RESPOND_MODEL_POSTSIM_HPP_