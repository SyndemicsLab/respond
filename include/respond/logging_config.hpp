////////////////////////////////////////////////////////////////////////////////
// File: logging_config.hpp                                                   //
// Project: respond                                                           //
// Created Date: 2026-09-22                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-09-22                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_LOGGING_CONFIG_HPP_
#define RESPOND_LOGGING_CONFIG_HPP_

#include <string>

#include <respond/constants.hpp>

namespace respond {
/// @brief Describes the logging destination used by a library object.
struct LoggingConfig {
    std::string logger_name = RESPOND_DEFAULT_LOG;
    std::string file_path = RESPOND_DEFAULT_LOG_FILE;
    bool use_shared_sink = false;
};
} // namespace respond

#endif // RESPOND_LOGGING_CONFIG_HPP_
