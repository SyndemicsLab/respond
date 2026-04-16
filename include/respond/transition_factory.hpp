////////////////////////////////////////////////////////////////////////////////
// File: transition_factory.hpp                                               //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TRANSITION_FACTORY_HPP_
#define RESPOND_TRANSITION_FACTORY_HPP_

#include <memory>

#include <respond/transition.hpp>

namespace respond {
/// @brief Factory for creating concrete Transition instances.
/// This factory supports creation of various transition types used in the
/// RESPOND model.
class TransitionFactory {
public:
    /// @brief Creates a transition of the specified type.
    /// @param type The type of transition to create. Supported types
    /// (case-insensitive):
    ///        - "migration": Population migration transitions
    ///        - "behavior": Behavioral state transitions
    ///        - "intervention": Intervention-driven transitions
    ///        - "overdose": Overdose-related transitions
    ///        - "background_death": Background mortality transitions
    /// @param log_name The logger name for error reporting (e.g., "console").
    /// @return A unique_ptr to the created Transition, or nullptr if type is
    /// unsupported.
    static std::unique_ptr<Transition>
    CreateTransition(const std::string &type, const std::string &log_name);
};
} // namespace respond

#endif // RESPOND_TRANSITION_FACTORY_HPP_