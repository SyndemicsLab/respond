////////////////////////////////////////////////////////////////////////////////
// File: runtime_config.hpp                                                    //
// Project: respond                                                           //
// Created Date: 2026-09-22                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_RUNTIME_CONFIG_HPP_
#define RESPOND_RUNTIME_CONFIG_HPP_

#include <respond/execution_config.hpp>
#include <respond/logging_config.hpp>

namespace respond {
/// @brief Groups execution and logging settings shared by runtime objects.
struct RuntimeConfig {
    ExecutionConfig execution;
    LoggingConfig logging;
};
} // namespace respond

#endif // RESPOND_RUNTIME_CONFIG_HPP_
