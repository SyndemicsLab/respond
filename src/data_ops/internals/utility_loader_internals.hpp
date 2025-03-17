////////////////////////////////////////////////////////////////////////////////
// File: utility_loader_internals.hpp                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-12                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_UTILITYLOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_UTILITYLAODERINTERNALS_HPP_

#include <respond/data_ops/utility_loader.hpp>

#include <memory>
#include <string>
#include <unordered_map>

#include <respond/data_ops/matrices.hpp>

#include "base_loader_internals.hpp"

namespace respond::data_ops {
    class UtilityLoaderImpl : public virtual UtilityLoader, public BaseLoader {
    public:
        UtilityLoaderImpl(const std::string &directory = "",
                          const std::string &log_name = "console")
            : BaseLoader(directory, log_name) {}

        ~UtilityLoaderImpl() {};

        std::unordered_map<std::string, Matrix3d>
        LoadBackgroundUtility(const std::string &file) override;

        std::unordered_map<std::string, Matrix3d>
        LoadOUDUtility(const std::string &file) override;

        std::unordered_map<std::string, Matrix3d>
        LoadSettingUtility(const std::string &file) override;

        Matrix3d
        GetBackgroundUtility(const std::string &perspective) const override {
            if (background_utility.find(perspective) !=
                background_utility.end()) {
                return background_utility.at(perspective);
            }

            // add warning
            Matrix3d result;
            return result;
        }

        Matrix3d GetOUDUtility(const std::string &perspective) const override {
            if (behavior_utility.find(perspective) != behavior_utility.end()) {
                return behavior_utility.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        Matrix3d
        GetSettingUtility(const std::string &perspective) const override {
            if (setting_utility.find(perspective) != setting_utility.end()) {
                return setting_utility.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

    private:
        std::unordered_map<std::string, Matrix3d> background_utility;
        std::unordered_map<std::string, Matrix3d> behavior_utility;
        std::unordered_map<std::string, Matrix3d> setting_utility;

        std::unordered_map<std::string, Matrix3d>
        LoadUtility(const std::string &file);
    };
} // namespace respond::data_ops

#endif