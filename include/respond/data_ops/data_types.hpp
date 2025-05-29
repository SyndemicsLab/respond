////////////////////////////////////////////////////////////////////////////////
// File: data_types.hpp                                                       //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATATYPES_HPP_
#define RESPOND_DATAOPS_DATATYPES_HPP_

#include <string>
#include <vector>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

/// @brief General Namespace for all RESPOND repo related code.
namespace respond {
/// @brief Namespace for all data operations.
namespace data_ops {

/// @brief Type alias for a 3 Dimensional Eigen Tensor
using Matrix3d = Eigen::Tensor<double, 3>;

/// @brief Type alias for 3 Dimensional Eigen Tensors given time points
using TimedMatrix3d = std::map<int, Matrix3d>;

/// @brief Type alias for a string map
/// @tparam T Type of key for the map
template <typename T> using StringMap = std::map<std::string, T>;

/// @brief Type alias for a string unordered map
/// @tparam T Type of key for the unordered map
template <typename T> using StringUOMap = std::unordered_map<std::string, T>;

/// @brief Specification for each dimension in the Matrix3d
enum class Dimension : int {
    kIntervention = 0,    // Intervention Dimension
    kOud = 1,             // OUD Dimension
    kDemographicCombo = 2 // Demographic Combination Dimension
};

/// @brief Struct grouping together matrices containing run history
struct History {
    TimedMatrix3d state_history;
    TimedMatrix3d overdose_history;
    TimedMatrix3d fatal_overdose_history;
    TimedMatrix3d mortality_history;
    TimedMatrix3d intervention_admission_history;
};

/// @brief Struct grouping together matrices containing cost information
struct Cost {
    std::string perspective;
    TimedMatrix3d healthcare_cost;
    TimedMatrix3d non_fatal_overdose_cost;
    TimedMatrix3d fatal_overdose_cost;
    TimedMatrix3d pharma_cost;
    TimedMatrix3d treatment_cost;
};

/// @brief Type alias for a vector of Cost objects
using CostList = std::vector<Cost>;

/// @brief Enum for the ways we can calculate utility
enum class UtilityType : int {
    kMin = 0,  // Calculate Minimum
    kMult = 1, // Calculate Multiplicative
    kCount = 2 // Counter for Enum
};

/// @brief Struct grouping together matricies containing total cost
/// effectiveness information
struct Totals {
    std::vector<double> base_costs;
    std::vector<double> disc_costs;
    double base_life_years = 0.0;
    double disc_life_years = 0.0;
    double base_utility = 0.0;
    double disc_utility = 0.0;
};

} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_DATATYPES_HPP_
