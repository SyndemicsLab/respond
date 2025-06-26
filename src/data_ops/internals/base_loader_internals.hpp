////////////////////////////////////////////////////////////////////////////////
// File: base_loader_internals.hpp                                            //
// Project: internals                                                         //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_

#include <algorithm>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <respond/utils/logging.hpp>

#include <respond/data_ops/base_loader.hpp>

namespace respond {
namespace data_ops {
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
} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_