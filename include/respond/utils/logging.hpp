////////////////////////////////////////////////////////////////////////////////
// File: logging.hpp                                                          //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-10                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-11                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_UTILS_LOGGING_HPP_
#define RESPOND_UTILS_LOGGING_HPP_

#include <string>

namespace respond::utils {
    void LogInfo(const std::string &logger_name, const std::string &message);
    void LogWarning(const std::string &logger_name, const std::string &message);
    void LogError(const std::string &logger_name, const std::string &message);
    void LogDebug(const std::string &logger_name, const std::string &message);

} // namespace respond::utils

#endif // RESPOND_UTILS_LOGGING_HPP_