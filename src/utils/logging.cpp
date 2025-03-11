////////////////////////////////////////////////////////////////////////////////
// File: logging.cpp                                                          //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-10                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-11                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/utils/logging.hpp>

#include "internals/logging_internals.hpp"

namespace respond::utils {
    void LogInfo(const std::string &logger_name, const std::string &message) {
        log(logger_name, message, LogType::kInfo);
    }

    void LogWarning(const std::string &logger_name,
                    const std::string &message) {
        log(logger_name, message, LogType::kWarn);
    }

    void LogError(const std::string &logger_name, const std::string &message) {
        log(logger_name, message, LogType::kError);
    }

    void LogDebug(const std::string &logger_name, const std::string &message) {
        log(logger_name, message, LogType::kDebug);
    }
} // namespace respond::utils