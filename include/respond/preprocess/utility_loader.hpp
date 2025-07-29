////////////////////////////////////////////////////////////////////////////////
// File: utility_loader.hpp                                                   //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <respond/utils/types.hpp>

#include <respond/preprocess/base_loader.hpp>

namespace respond {
namespace preprocess {
/// @brief A class to load Utility Data from files.
class UtilityLoader : public virtual BaseLoader {
public:
    /// @brief Default Deconstructor for the UtilityLoader class.
    virtual ~UtilityLoader() = default;

    /// @brief Load the Background Utility Data from a file.
    /// @param file File path to background utility data.
    /// @return An unordered map of perspective names to utility matrices.
    virtual StringUOMap<Matrix3d>
    LoadBackgroundUtility(const std::string &file) = 0;

    /// @brief Load the OUD Utility Data from a file.
    /// @param file File path to OUD utility data.
    /// @return An unordered map of perspective names to utility matrices.
    virtual StringUOMap<Matrix3d> LoadOUDUtility(const std::string &file) = 0;

    /// @brief Load the Setting Utility Data from a file.
    /// @param file File path to setting utility data.
    /// @return An unordered map of perspective names to utility matrices.
    virtual StringUOMap<Matrix3d>
    LoadSettingUtility(const std::string &file) = 0;

    /// @brief Get the Background Utility for a specified perspective.
    /// @param perspective Perspective name to get the utility for.
    /// @return A 3D Eigen Tensor representing the background utility.
    virtual Matrix3d
    GetBackgroundUtility(const std::string &perspective) const = 0;

    /// @brief Get the OUD Utility for a specified perspective.
    /// @param perspective Perspective name to get the utility for.
    /// @return A 3D Eigen Tensor representing the OUD utility.
    virtual Matrix3d GetOUDUtility(const std::string &perspective) const = 0;

    /// @brief Get the Setting Utility for a specified perspective.
    /// @param perspective Perspective name to get the utility for.
    /// @return A 3D Eigen Tensor representing the setting utility.
    virtual Matrix3d
    GetSettingUtility(const std::string &perspective) const = 0;

    /// @brief Factory method to create an instance of a UtilityLoader.
    /// @param directory Directory to load utility data from.
    /// @param log_name Name of log file for logging messages.
    /// @return A unique pointer to a UtilityLoader.
    static std::unique_ptr<UtilityLoader>
    Create(const std::string &log_name = "console");
};
} // namespace preprocess
} // namespace respond

#endif