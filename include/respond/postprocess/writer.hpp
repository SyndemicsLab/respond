////////////////////////////////////////////////////////////////////////////////
// File: writer.hpp                                                           //
// Project: preprocess // Created Date: 2025-07-07 // Author: Matthew Carroll //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_POSTPROCESS_WRITER_HPP_
#define RESPOND_POSTPROCESS_WRITER_HPP_

#include <filesystem>
#include <memory>
#include <string>

#include <respond/preprocess/data_loader.hpp>
#include <respond/utils/types.hpp>

namespace respond {
namespace preprocess {

/// @brief Enum declaring the different types of writers available.
enum class WriterType : int {
    kInput = 0,     // Input Data
    kOutput = 1,    // Output Data
    kHistory = 2,   // History Data
    kCost = 3,      // Cost Data
    kUtilities = 4, // Utility Data
    kTotals = 5,    // Totals Data
    kCount = 6      // Count of Writer Types
};

/// @brief Enum declaring the different output types the writer can use.
enum class OutputType : int {
    kString = 0, // Output as a string
    kFile = 1,   // Output to a file
    kCount = 2   // Enum counter
};

/// @brief Class for writing all information from the model.
class Writer {
public:
    /// @brief Default destructor for the Writer class.
    virtual ~Writer() = default;

    /// @brief Function used to write the input data to the specified directory.
    /// @param data_loader data loader containing the data to write.
    /// @param directory Directory to place the input data.
    /// @param output_type Output type to write the data.
    /// @return A string containing the result of the write.
    virtual std::string WriteInputData(const DataLoader &data_loader,
                                       const std::string &directory,
                                       const OutputType output_type) const = 0;

    /// @brief Function used to write the history data to the specified
    /// directory.
    /// @param history History data to write.
    /// @param directory Directory to place the history data.
    /// @param output_type Output type to write the data.
    /// @return A string containing the result of the write.
    virtual std::string
    WriteHistoryData(const History &history, const std::string &directory,
                     const OutputType output_type) const = 0;

    /// @brief Function used to write the cost data to the specified directory.
    /// @param cost_list List of cost data to write.
    /// @param directory Directory to place the cost data.
    /// @param output_type Output type to write the data.
    /// @return A string containing the result of the write.
    virtual std::string WriteCostData(const CostList &cost_list,
                                      const std::string &directory,
                                      const OutputType output_type) const = 0;

    /// @brief Function used to write the utility data to the specified
    /// directory.
    /// @param utilities Utility data to write.
    /// @param directory Directory to place the utility data.
    /// @param output_type Output type to write the data.
    /// @return A string containing the result of the write.
    virtual std::string
    WriteUtilityData(const TimedMatrix3d &utilities,
                     const std::string &directory,
                     const OutputType output_type) const = 0;

    /// @brief Function used to write the totals data to the specified
    /// directory.
    /// @param totals Totals data to write.
    /// @param directory Directory to place the totals data.
    /// @param output_type Output type to write the data.
    /// @return A string containing the result of the write.
    virtual std::string WriteTotalsData(const Totals &totals,
                                        const std::string &directory,
                                        const OutputType output_type) const = 0;

    /// @brief A factory method to create a Writer instance.
    /// @param directory Directory to read the config information from.
    /// @param log_name Name of the logger you want to use for logging.
    /// @return An instance of a Writer.
    static std::unique_ptr<Writer>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};

} // namespace preprocess
} // namespace respond

#endif // RESPOND_POSTPROCESS_WRITER_HPP_