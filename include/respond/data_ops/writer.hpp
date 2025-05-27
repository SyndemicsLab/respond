////////////////////////////////////////////////////////////////////////////////
// File: Writer.hpp                                                           //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_WRITER_HPP_
#define RESPOND_DATAOPS_WRITER_HPP_

#include <filesystem>
#include <memory>
#include <string>

#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond {
namespace data_ops {

/// @brief
enum class WriterType : int {
    kInput = 0,     // Input Data
    kOutput = 1,    // Output Data
    kHistory = 2,   // History Data
    kCost = 3,      // Cost Data
    kUtilities = 4, // Utility Data
    kTotals = 5,    // Totals Data
    kCount = 6      // Count of Writer Types
};

/// @brief
enum class OutputType : int {
    kString = 0, // Output as a string
    kFile = 1,   // Output to a file
    kCount = 2   // Enum counter
};

/// @brief
class Writer {
public:
    /// @brief
    virtual ~Writer() = default;

    /// @brief
    /// @param data_loader
    /// @param directory
    /// @param output_type
    /// @return
    virtual std::string WriteInputData(const DataLoader &data_loader,
                                       const std::string &directory,
                                       const OutputType output_type) const = 0;

    /// @brief
    /// @param history
    /// @param directory
    /// @param output_type
    /// @return
    virtual std::string
    WriteHistoryData(const History &history, const std::string &directory,
                     const OutputType output_type) const = 0;

    /// @brief
    /// @param cost_list
    /// @param directory
    /// @param output_type
    /// @return
    virtual std::string WriteCostData(const CostList &cost_list,
                                      const std::string &directory,
                                      const OutputType output_type) const = 0;

    /// @brief
    /// @param utilities
    /// @param directory
    /// @param output_type
    /// @return
    virtual std::string
    WriteUtilityData(const TimedMatrix3d &utilities,
                     const std::string &directory,
                     const OutputType output_type) const = 0;

    /// @brief
    /// @param totals
    /// @param directory
    /// @param output_type
    /// @return
    virtual std::string WriteTotalsData(const Totals &totals,
                                        const std::string &directory,
                                        const OutputType output_type) const = 0;

    /// @brief
    /// @param directory
    /// @param log_name
    /// @return
    static std::unique_ptr<Writer>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};

} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_WRITER_HPP_