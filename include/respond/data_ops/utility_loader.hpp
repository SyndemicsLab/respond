////////////////////////////////////////////////////////////////////////////////
// File: UtilityLoader.hpp                                                    //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include <string>
#include <unordered_map>

#include <respond/data_ops/matrices.hpp>

namespace respond::data_ops {
    class UtilityLoader {
    public:
        /// @brief Load the Background Utilties from a File
        /// @param csvName Filename containing Background Utility
        /// @return Matrix3d of Background Utility
        virtual std::unordered_map<std::string, Matrix3d>
        LoadBackgroundUtility(std::string const &csvName) = 0;

        /// @brief Load the OUD Utility from a File
        /// @param csvName Filename containing OUD Utility
        /// @return Matrix3d of OUD Utility
        virtual std::unordered_map<std::string, Matrix3d>
        LoadOUDUtility(std::string const &csvName) = 0;

        /// @brief Load the Setting Utility from a File
        /// @param csvName Filename containing the Setting Utility
        /// @return Matrix3d of Setting Utility
        virtual std::unordered_map<std::string, Matrix3d>
        LoadSettingUtility(std::string const &csvName) = 0;

        // GETTERS
        /// @brief Get the Background Utility
        /// @return Matrix3d of Background Utility
        virtual Matrix3d
        GetBackgroundUtility(std::string const &perspective) const = 0;

        /// @brief Get the OUD Utility
        /// @return Matrix3d of OUD Utility
        virtual Matrix3d
        GetOUDUtility(std::string const &perspective) const = 0;

        /// @brief Get the Setting Utility
        /// @return Matrix3d of Setting Utility
        virtual Matrix3d
        GetSettingUtility(std::string const &perspective) const = 0;
    };
} // namespace respond::data_ops

#endif