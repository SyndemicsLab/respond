////////////////////////////////////////////////////////////////////////////////
// File: helpers.hpp                                                          //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MODEL_HELPERS_HPP_
#define RESPOND_MODEL_HELPERS_HPP_

#include <iostream>
#include <string>
#include <vector>

#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>

namespace respond::model {
    std::vector<double>
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

#endif // RESPOND_MODEL_HELPERS_HPP_
