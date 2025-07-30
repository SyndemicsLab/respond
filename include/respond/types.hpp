////////////////////////////////////////////////////////////////////////////////
// File: types.hpp                                                            //
// Project: respond                                                           //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATATYPES_HPP_
#define RESPOND_DATATYPES_HPP_

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <Eigen/Dense>

/// @brief General Namespace for all RESPOND repo related code.
namespace respond {

/// @brief Type alias for a string map
/// @tparam T Type of key for the map
template <typename T> using StringMap = std::map<std::string, T>;

/// @brief Type alias for a string unordered map
/// @tparam T Type of key for the unordered map
template <typename T> using StringUOMap = std::unordered_map<std::string, T>;

/// @brief Struct grouping together matrices containing run history
struct HistoryStamp {
    Eigen::VectorXd state;
    Eigen::VectorXd overdoses;
    Eigen::VectorXd intervention_admissions;
};

using HistoryOverTime = std::map<int, HistoryStamp>;

/// @brief Struct grouping together matrices containing cost information
struct CostStamp {
    Eigen::VectorXd healthcare;
    Eigen::VectorXd non_fatal_overdoses;
    Eigen::VectorXd fatal_overdoses;
    Eigen::VectorXd pharmaceuticals;
    Eigen::VectorXd treatments;
};

using CostsOverTime = std::map<int, CostStamp>;

/// @brief Type alias for a vector of Cost objects
using CostPerspectives = std::map<std::string, CostsOverTime>;

/// @brief Enum for the ways we can calculate utility
enum class UtilityType : int {
    kMin = 0,  // Calculate Minimum
    kMult = 1, // Calculate Multiplicative
    kCount = 2 // Counter for Enum
};

struct ResultSets {
    std::vector<double> summed_costs;
    double summed_life_years = 0.0;
    double summed_utility = 0.0;
};

/// @brief Struct grouping together matricies containing total cost
/// effectiveness information
struct Totals {
    ResultSets base;
    ResultSets discounted;
};

} // namespace respond

#endif // RESPOND_DATATYPES_HPP_
