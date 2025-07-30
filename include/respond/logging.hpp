////////////////////////////////////////////////////////////////////////////////
// File: logging.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_LOGGING_HPP_
#define RESPOND_LOGGING_HPP_

#include <string>

namespace respond {

/// @brief Logging levels for the logger.
enum class LogType : int {
    kInfo = 0,  // Information Messages
    kWarn = 1,  // Warning Messages
    kError = 2, // Error Messages
    kDebug = 3, // Debug Messages
    kCount = 4  // Enum Counter
};

/// @brief Creation status of the logger.
enum class CreationStatus : int {
    kError = -1,     // Error Creating Logger
    kSuccess = 0,    // Logger Created Successfully
    kExists = 1,     // Logger Already Exists
    kNotCreated = 2, // Logger Not Created
    kCount = 4       // Enum Counter
};

/// @brief Create a logger with the specified name and file path.
/// @param logger_name logger name to be created.
/// @param filepath file path where the logger will write logs.
/// @return CreationStatus indicating the result of the logger creation.
CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath);

/// @brief Log a message as information.
/// @param logger_name Logger name to log the message to.
/// @param message Message to log as information.
void LogInfo(const std::string &logger_name, const std::string &message);

/// @brief Log as message as a warning.
/// @param logger_name Logger name to log the message to.
/// @param message Message to log as a warning.
void LogWarning(const std::string &logger_name, const std::string &message);

/// @brief Log a message as an error.
/// @param logger_name Logger name to log the message to.
/// @param message Message to log as an error.
void LogError(const std::string &logger_name, const std::string &message);

/// @brief Log a message as debug information.
/// @param logger_name Logger name to log the message to.
/// @param message Message to log as debug information.
void LogDebug(const std::string &logger_name, const std::string &message);

} // namespace respond

#endif // RESPOND_LOGGING_HPP_