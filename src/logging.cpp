////////////////////////////////////////////////////////////////////////////////
// File: logging.cpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/logging.hpp>

#include "internals/logging_internals.hpp"

namespace respond {
CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath) {
    if (CheckIfExists(logger_name) == CreationStatus::kExists) {
        return CreationStatus::kExists;
    }
    try {
        spdlog::cfg::load_env_levels();
        spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
        spdlog::flush_every(std::chrono::seconds(3));
        spdlog::basic_logger_mt(logger_name, filepath);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return CreationStatus::kError;
    }
    return CreationStatus::kSuccess;
}

void LogInfo(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kInfo);
}

void LogWarning(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kWarn);
}

void LogError(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kError);
}

void LogDebug(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kDebug);
}
} // namespace respond