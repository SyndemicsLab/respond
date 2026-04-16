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

#include <chrono>
#include <iostream>

namespace respond {

CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath) {
    if (CheckIfExists(logger_name) == CreationStatus::kExists) {
        std::cout << "Logger " << logger_name << " already exists" << std::endl;
        return CreationStatus::kExists;
    }
    try {
        spdlog::cfg::load_env_levels();
        std::string pattern =
            LoggingConfig::GetPatternString(LoggingConfig::GetPattern());
        spdlog::set_pattern(pattern);
        int flush_interval = LoggingConfig::GetFlushInterval();
        if (flush_interval > 0) {
            spdlog::flush_every(std::chrono::seconds(flush_interval));
        }
        spdlog::basic_logger_mt(logger_name, filepath);
    } catch (const spdlog::spdlog_ex &ex) {
        std::string error_msg = "Failed to create file logger '" + logger_name +
                                "' at path '" + filepath + "': " + ex.what();
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    }
    return CreationStatus::kSuccess;
}

CreationStatus CreateSharedFileSink(const std::string &filepath) {
    try {
        auto sink = LoggingConfig::GetSharedSink(filepath);
        if (sink) {
            LoggingConfig::SetDefaultSinkPath(filepath);
            return CreationStatus::kSuccess;
        }
        std::string error_msg =
            "Failed to create shared file sink: sink is null";
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    } catch (const spdlog::spdlog_ex &ex) {
        std::string error_msg = "Failed to create shared file sink at '" +
                                filepath + "': " + ex.what();
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    }
}

CreationStatus CreateSharedLogger(const std::string &logger_name) {
    if (CheckIfExists(logger_name) == CreationStatus::kExists) {
        std::cout << "Shared logger " << logger_name << " already exists"
                  << std::endl;
        return CreationStatus::kExists;
    }

    try {
        std::string filepath = LoggingConfig::GetDefaultSinkPath();
        auto sink = LoggingConfig::GetSharedSink(filepath);
        if (!sink) {
            std::string error_msg =
                "Failed to create shared logger '" + logger_name +
                "': could not get or create shared sink at '" + filepath + "'";
            std::cerr << error_msg << std::endl;
            return CreationStatus::kError;
        }

        spdlog::cfg::load_env_levels();
        std::string pattern =
            LoggingConfig::GetPatternString(LoggingConfig::GetPattern());

        auto logger = std::make_shared<spdlog::logger>(logger_name, sink);
        logger->set_pattern(pattern);
        logger->set_level(spdlog::level::trace);

        spdlog::register_logger(logger);

        int flush_interval = LoggingConfig::GetFlushInterval();
        if (flush_interval > 0) {
            spdlog::flush_every(std::chrono::seconds(flush_interval));
        }

        return CreationStatus::kSuccess;
    } catch (const spdlog::spdlog_ex &ex) {
        std::string error_msg = "Failed to create shared logger '" +
                                logger_name + "': " + ex.what();
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    }
}

void SetLogPattern(LogPattern pattern) { LoggingConfig::SetPattern(pattern); }

LogPattern GetLogPattern() { return LoggingConfig::GetPattern(); }

void SetFlushInterval(int seconds) {
    LoggingConfig::SetFlushInterval(seconds);
    if (seconds > 0) {
        spdlog::flush_every(std::chrono::seconds(seconds));
    }
}

void FlushAllLoggers() {
    spdlog::apply_all(
        [](std::shared_ptr<spdlog::logger> log) { log->flush(); });
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

CreationStatus CheckLoggerExists(const std::string &logger_name) {
    return CheckIfExists(logger_name);
}

std::string GetLoggerInfo(const std::string &logger_name) {
    auto logger = spdlog::get(logger_name);
    if (!logger) {
        return "Logger \"" + logger_name + "\" not found";
    }

    auto level_view = spdlog::level::to_string_view(logger->level());
    std::string info = "Logger: " + logger_name + "\n";
    info +=
        "  Level: " + std::string(level_view.data(), level_view.size()) + "\n";
    info += "  Sinks: " + std::to_string(logger->sinks().size());
    return info;
}

void SetLoggerLevel(const std::string &logger_name, int level) {
    auto logger = spdlog::get(logger_name);
    if (logger) {
        int clamped_level = (level < 0) ? 0 : (level > 5) ? 5 : level;
        logger->set_level(
            static_cast<spdlog::level::level_enum>(clamped_level));
    }
}

} // namespace respond