////////////////////////////////////////////////////////////////////////////////
// File: transition_factory.cpp                                               //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/transition_factory.hpp>

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>

#include "internals/background.hpp"
#include "internals/behavior.hpp"
#include "internals/intervention.hpp"
#include "internals/migration.hpp"
#include "internals/overdose.hpp"

namespace respond {
std::unique_ptr<Transition>
TransitionFactory::CreateTransition(const std::string &type,
                                    const std::string &log_name) {
    std::string type_copy = type;
    std::transform(type_copy.begin(), type_copy.end(), type_copy.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (type == "migration") {
        return Migration::Create(type, log_name);
    } else if (type == "behavior") {
        return Behavior::Create(type, log_name);
    } else if (type == "intervention") {
        return Intervention::Create(type, log_name);
    } else if (type == "overdose") {
        return Overdose::Create(type, log_name);
    } else if (type == "background_death") {
        return BackgroundDeath::Create(type, log_name);
    }
    // Warn invalid type
    return nullptr;
}
} // namespace respond