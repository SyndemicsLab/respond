////////////////////////////////////////////////////////////////////////////////
// File: data_types.hpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATATYPES_HPP_
#define RESPOND_DATAOPS_DATATYPES_HPP_

#include <string>
#include <vector>

#include <respond/data_ops/matrices.hpp>

namespace respond::data_ops {

    /// @brief Specification for each dimension in the Matrix3d
    enum class Dimension : int {
        kIntervention = 0,
        kOud = 1,
        kDemographicCombo = 2
    };
    struct History {
        TimedMatrix3d state_history;
        TimedMatrix3d overdose_history;
        TimedMatrix3d fatal_overdose_history;
        TimedMatrix3d mortality_history;
        TimedMatrix3d intervention_admission_history;
    };

    /// @brief Struct defining all the History Matrices Across the Duration of
    /// the Simulation
    using History = History;

    struct Cost {
        std::string perspective;
        TimedMatrix3d healthcare_cost;
        TimedMatrix3d non_fatal_overdose_cost;
        TimedMatrix3d fatal_overdose_cost;
        TimedMatrix3d pharma_cost;
        TimedMatrix3d treatment_cost;
    };

    /// @brief
    using Cost = Cost;

    /// @brief Struct defining Cost Matrices Across the Duration of the
    /// Simulation
    using CostList = std::vector<Cost>;

    enum class UtilityType : int { kMin = 0, kMult = 1, kCount = 2 };

    struct Totals {
        std::vector<double> base_costs;
        std::vector<double> disc_costs;
        double base_life_years = 0.0;
        double disc_life_years = 0.0;
        double base_utility = 0.0;
        double disc_utility = 0.0;
    };

    using Totals = Totals;

}; // namespace respond::data_ops

#endif // RESPOND_DATAOPS_DATATYPES_HPP_
