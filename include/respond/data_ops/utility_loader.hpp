////////////////////////////////////////////////////////////////////////////////
// File: UtilityLoader.hpp                                                    //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <respond/data_ops/data_types.hpp>

namespace respond {
namespace data_ops {
/// @brief
class UtilityLoader {
public:
    /// @brief
    /// @param file
    /// @return
    virtual StringUOMap<Matrix3d>
    LoadBackgroundUtility(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual StringUOMap<Matrix3d> LoadOUDUtility(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual StringUOMap<Matrix3d>
    LoadSettingUtility(const std::string &file) = 0;

    /// @brief
    /// @param perspective
    /// @return
    virtual Matrix3d
    GetBackgroundUtility(const std::string &perspective) const = 0;

    /// @brief
    /// @param perspective
    /// @return
    virtual Matrix3d GetOUDUtility(const std::string &perspective) const = 0;

    /// @brief
    /// @param perspective
    /// @return
    virtual Matrix3d
    GetSettingUtility(const std::string &perspective) const = 0;

    /// @brief
    /// @param directory
    /// @param log_name
    /// @return
    static std::unique_ptr<UtilityLoader>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};
} // namespace data_ops
} // namespace respond

#endif