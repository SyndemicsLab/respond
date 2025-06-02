////////////////////////////////////////////////////////////////////////////////
// File: data_formatter.hpp                                                   //
// Project: data_ops                                                          //
// Created Date: 2025-03-12                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_DATAFORMATTER_HPP_
#define RESPOND_DATAOPS_DATAFORMATTER_HPP_

#include <memory>
#include <vector>

#include <respond/data_ops/data_types.hpp>

namespace respond {
namespace data_ops {
/// @brief Class to handle the data formatting needs for RESPOND.
class DataFormatter {
public:
    /// @brief Default constructor for the DataFormatter class.
    virtual ~DataFormatter() = default;

    /// @brief Core functionality of the class. Extracts the content for each
    /// specified timestep for each reference.
    /// @param timesteps Timesteps to extract.
    /// @param history History object containing the model run data.
    /// @param costs Cost data calculated throughout the model run.
    /// @param utilities Utility data calculated throughout the model run.
    /// @param costSwitch Boolean to indicate if we use costs.
    virtual void ExtractTimesteps(std::vector<int> &timesteps, History &history,
                                  CostList &costs, TimedMatrix3d &utilities,
                                  bool costSwitch) = 0;

    /// @brief Factory method to create an instance of DataFormatter.
    /// @return An instance of DataFormatter.
    static std::unique_ptr<DataFormatter> Create();
};
} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_DATAFORMATTER_HPP_