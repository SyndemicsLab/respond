////////////////////////////////////////////////////////////////////////////////
// File: base_loader_internals.hpp                                            //
// Project: internals                                                         //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_PREPROCESS_BASELOADERINTERNALS_HPP_
#define RESPOND_PREPROCESS_BASELOADERINTERNALS_HPP_

#include <algorithm>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <respond/utils/logging.hpp>

#include <respond/preprocess/base_loader.hpp>

namespace respond {
namespace preprocess {
class BaseLoaderImpl : public virtual BaseLoader {
public:
    BaseLoaderImpl(const std::string &log_name = "console")
        : logger_name(log_name) {}

    Data::IDataTablePtr LoadDataTable(const std::string &path,
                                      bool headers = true) {
        return std::make_shared<Data::DataTable>(path, headers, ',');
    }

    void SetConfig(const std::string &config_file) {
        if (std::filesystem::exists(config_file)) {
            config = std::make_shared<Data::Config>(config_file);
        } else {
            respond::utils::LogError(logger_name, "Config file not found at " +
                                                      config_file +
                                                      ". Expect Errors!");
        }
    }

    Data::IConfigablePtr GetConfig() const { return config; }

    [[deprecated("Will always return {\"na\"}.")]]
    std::vector<std::string> GetDemographicCombos() const {
        return {"na"};
    }

    [[deprecated("No operation performed.")]]
    void SetDemographicCombos(const std::vector<std::string> &combos) {
        return;
    }

protected:
    Data::IConfigablePtr config = nullptr;
    const std::string logger_name;
};
} // namespace preprocess
} // namespace respond

#endif // RESPOND_PREPROCESS_BASELOADERINTERNALS_HPP_