////////////////////////////////////////////////////////////////////////////////
// File: logging.hpp                                                          //
// Project: utils                                                             //
// Created Date: 2025-03-10                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_UTILS_LOGGING_HPP_
#define RESPOND_UTILS_LOGGING_HPP_

#include <string>

namespace respond {
/// @brief
namespace utils {

/// @brief
enum class LogType : int {
    kInfo = 0,  // Information Messages
    kWarn = 1,  // Warning Messages
    kError = 2, // Error Messages
    kDebug = 3, // Debug Messages
    kCount = 4  // Enum Counter
};

/// @brief
enum class CreationStatus : int {
    kError = -1,     // Error Creating Logger
    kSuccess = 0,    // Logger Created Successfully
    kExists = 1,     // Logger Already Exists
    kNotCreated = 2, // Logger Not Created
    kCount = 4       // Enum Counter
};

/// @brief
/// @param logger_name
/// @param filepath
/// @return
CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath);

/// @brief
/// @param logger_name
/// @param message
void LogInfo(const std::string &logger_name, const std::string &message);

/// @brief
/// @param logger_name
/// @param message
void LogWarning(const std::string &logger_name, const std::string &message);

/// @brief
/// @param logger_name
/// @param message
void LogError(const std::string &logger_name, const std::string &message);

/// @brief
/// @param logger_name
/// @param message
void LogDebug(const std::string &logger_name, const std::string &message);

} // namespace utils
} // namespace respond

#endif // RESPOND_UTILS_LOGGING_HPP_