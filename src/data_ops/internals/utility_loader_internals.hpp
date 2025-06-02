////////////////////////////////////////////////////////////////////////////////
// File: utility_loader_internals.hpp                                         //
// Project: internals                                                         //
// Created Date: 2025-03-12                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
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

#include <respond/data_ops/data_types.hpp>

#include "base_loader_internals.hpp"

namespace respond {
namespace data_ops {
class UtilityLoaderImpl : public virtual UtilityLoader, public BaseLoader {
public:
    UtilityLoaderImpl(const std::string &directory = "",
                      const std::string &log_name = "console")
        : BaseLoader(directory, log_name) {}

    ~UtilityLoaderImpl() {};

    StringUOMap<Matrix3d>
    LoadBackgroundUtility(const std::string &file) override;

    StringUOMap<Matrix3d> LoadOUDUtility(const std::string &file) override;

    StringUOMap<Matrix3d> LoadSettingUtility(const std::string &file) override;

    Matrix3d
    GetBackgroundUtility(const std::string &perspective) const override {
        if (background_utility.find(perspective) != background_utility.end()) {
            return background_utility.at(perspective);
        }
        respond::utils::LogWarning(
            logger_name,
            "Background Utility not found for perspective: " + perspective);
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

    Matrix3d GetSettingUtility(const std::string &perspective) const override {
        if (setting_utility.find(perspective) != setting_utility.end()) {
            return setting_utility.at(perspective);
        }
        // add warning
        Matrix3d result;
        return result;
    }

private:
    StringUOMap<Matrix3d> background_utility;
    StringUOMap<Matrix3d> behavior_utility;
    StringUOMap<Matrix3d> setting_utility;

    StringUOMap<Matrix3d> LoadUtility(const std::string &file);
};
} // namespace data_ops
} // namespace respond

#endif