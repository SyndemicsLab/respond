////////////////////////////////////////////////////////////////////////////////
// File: markov.cpp                                                           //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "internals/markov.hpp"

#include <memory>
#include <string>

#include <respond/logging.hpp>
#include <respond/model.hpp>

namespace respond {

std::unique_ptr<Model> Model::Create(const std::string &name,
                                     const std::string &log_name) {
    return std::make_unique<Markov>(name, log_name);
}
} // namespace respond
