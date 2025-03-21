////////////////////////////////////////////////////////////////////////////////
// File: UtilityLoader.hpp                                                    //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <respond/data_ops/matrices.hpp>

namespace respond::data_ops {
    class UtilityLoader {
    public:
        virtual std::unordered_map<std::string, Matrix3d>
        LoadBackgroundUtility(const std::string &file) = 0;

        virtual std::unordered_map<std::string, Matrix3d>
        LoadOUDUtility(const std::string &file) = 0;

        virtual std::unordered_map<std::string, Matrix3d>
        LoadSettingUtility(const std::string &file) = 0;

        virtual Matrix3d
        GetBackgroundUtility(const std::string &perspective) const = 0;

        virtual Matrix3d
        GetOUDUtility(const std::string &perspective) const = 0;

        virtual Matrix3d
        GetSettingUtility(const std::string &perspective) const = 0;

        static std::unique_ptr<UtilityLoader>
        Create(const std::string &directory = "",
               const std::string &log_name = "console");
    };
} // namespace respond::data_ops

#endif