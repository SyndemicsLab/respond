////////////////////////////////////////////////////////////////////////////////
// File: logging_internals.hpp                                                //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-10                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_UTILS_LOGGINGINTERNALS_HPP_
#define RESPOND_UTILS_LOGGINGINTERNALS_HPP_

#include <string>

#include <spdlog/spdlog.h>

namespace respond::utils {
    enum class LogType { kInfo, kWarn, kError, kDebug, kCount };

    void log(const std::string &logger_name, const std::string &message,
             LogType type = LogType::kInfo) {
        auto logger = spdlog::get(logger_name);
        if (logger) {
            switch (type) {
            case LogType::kInfo:
                logger->info(message);
                break;
            case LogType::kWarn:
                logger->warn(message);
                break;
            case LogType::kError:
                logger->error(message);
                break;
            case LogType::kDebug:
                logger->debug(message);
                break;
            default:
                logger->info(message);
                break;
            }
            logger->flush();
        } else {
            spdlog::error("Logger {} not found", logger_name);
        }
    }

} // namespace respond::utils

#endif // RESPOND_UTILS_LOGGINGINTERNALS_HPP_