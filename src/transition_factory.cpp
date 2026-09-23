////////////////////////////////////////////////////////////////////////////////
// File: transition_factory.cpp                                               //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>

#include <respond/logging.hpp>

#include "internals/background.hpp"
#include "internals/behavior.hpp"
#include "internals/intervention.hpp"
#include "internals/migration.hpp"
#include "internals/overdose.hpp"

namespace respond {
std::unique_ptr<Transition> Transition::Create(const std::string &type,
                                               const std::string &name,
                                               const std::string &log_name,
                                               const std::string &log_file) {
    return Create(type, name, LoggingConfig{log_name, log_file, false});
}

std::unique_ptr<Transition>
Transition::Create(const std::string &type, const std::string &name,
                  const LoggingConfig &logging_config) {
    std::string type_copy = type;
    std::transform(type_copy.begin(), type_copy.end(), type_copy.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (type_copy == "migration") {
        return std::make_unique<Migration>(name, logging_config);
    } else if (type_copy == "behavior") {
        return std::make_unique<Behavior>(name, logging_config);
    } else if (type_copy == "intervention") {
        return std::make_unique<Intervention>(name, logging_config);
    } else if (type_copy == "overdose") {
        return std::make_unique<Overdose>(name, logging_config);
    } else if (type_copy == "background_death") {
        return std::make_unique<BackgroundDeath>(name, logging_config);
    }

    // Invalid transition type
    std::string error_msg = "Invalid transition type: '" + type +
                            "'. Supported types: migration, behavior, "
                            "intervention, overdose, background_death";
    LogError(logging_config.logger_name, error_msg);
    throw std::invalid_argument(error_msg);
}
} // namespace respond