////////////////////////////////////////////////////////////////////////////////
// File: cost_loader.hpp                                                      //
// Project: preprocess // Created Date: 2025-07-07 // Author: Matthew Carroll //
// -----                                                                      //
// Last Modified: 2025-07-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_PREPROCESS_COSTLOADER_HPP_
#define RESPOND_PREPROCESS_COSTLOADER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <respond/utils/data_types.hpp>

#include <respond/preprocess/base_loader.hpp>

namespace respond {
namespace preprocess {
/// @brief Class to manage the Costing Data Necessary for RESPOND.
class CostLoader : public virtual BaseLoader {
public:
    /// @brief Default Deconstructor for the Cost Loader class.
    virtual ~CostLoader() = default;

    /// @brief Functionality to Load the data corresponding to healthcare
    /// utilization costs from a file.
    /// @param file A string representing the file path to the healthcare
    /// utilization cost data.
    /// @return An unordered map with string perspectives as keys and the
    /// corresponding Matrix3d values.
    virtual StringUOMap<Matrix3d>
    LoadHealthcareUtilizationCost(const std::string &file) = 0;

    /// @brief Functionality to Load the Data Corresponding to All Overdose
    /// Costs from a file.
    /// @param file File path to the overdose cost data.
    /// @return An unordered map of unordered maps containing the per overdose
    /// per perspective data
    virtual StringUOMap<StringUOMap<double>>
    LoadOverdoseCost(const std::string &file) = 0;

    /// @brief Functionality to Load the Data Corresponding to Pharmaceutical
    /// Costs from a file.
    /// @param file A string representing the file path to the pharmaceutical
    /// cost data.
    /// @return An unordered map with string perspectives as keys and the
    /// corresponding Matrix3d values.
    virtual StringUOMap<Matrix3d>
    LoadPharmaceuticalCost(const std::string &file) = 0;

    /// @brief Functionality to Load the Data Corresponding to Treatment
    /// Utilization Costs from a file.
    /// @param file A string representing the file path to the treatment
    /// utilization cost data.
    /// @return An unordered map with string perspectives as keys and the
    /// corresponding Matrix3d values.
    virtual StringUOMap<Matrix3d>
    LoadTreatmentUtilizationCost(const std::string &file) = 0;

    /// @brief Getter for Healthcare Utilization Costs.
    /// @param perspective Perspective to get.
    /// @return Matrix3d containing the costs for the given perspective.
    virtual Matrix3d
    GetHealthcareUtilizationCost(const std::string &perspective) const = 0;

    /// @brief Getter for Pharmaceutical Costs.
    /// @param perspective Perspective to get.
    /// @return Matrix3d containing the pharmaceutical costs for a given
    /// perspective.
    virtual Matrix3d
    GetPharmaceuticalCost(const std::string &perspective) const = 0;

    /// @brief Getter for Treatment Utilization Costs.
    /// @param perspective Perspective to get.
    /// @return Matrix3d containing the treatment utilization costs for a given
    /// perspective.
    virtual Matrix3d
    GetTreatmentUtilizationCost(const std::string &perspective) const = 0;

    /// @brief Getter for Non Fatal Overdose Costs.
    /// @param perspective Perspective to get.
    /// @return double containing the cost for a non-fatal overdose.
    virtual double
    GetNonFatalOverdoseCost(const std::string &perspective) const = 0;

    /// @brief Getter for Fatal Overdose Costs.
    /// @param perspective Perspective to get.
    /// @return double containing the cost for a fatal overdose.
    virtual double
    GetFatalOverdoseCost(const std::string &perspective) const = 0;

    /// @brief Factory function to create a CostLoader instance.
    /// @param directory Directory where the cost files are located.
    /// @param log_name Name of the logger to write to during errors.
    /// @return An instance of CostLoader.
    static std::unique_ptr<CostLoader>
    Create(const std::string &log_name = "console");
};
} // namespace preprocess
} // namespace respond

#endif // RESPOND_PREPROCESS_COSTLOADER_HPP_