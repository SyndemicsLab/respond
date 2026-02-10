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
class TransitionFactory {
public:
    static std::unique_ptr<Transition>
    CreateTransition(const std::string &type, const std::string &log_name);
};
} // namespace respond

#endif // RESPOND_TRANSITION_FACTORY_HPP_