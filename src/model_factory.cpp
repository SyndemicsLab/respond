////////////////////////////////////////////////////////////////////////////////
// File: model_factory.cpp                                                    //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-09-14                                                  //
// Modified By: Dimitri Baptiste                                              //
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
                                     const std::string &log_name,
                                     const std::string &log_filepath) {
    return std::make_unique<Markov>(name, log_name, log_filepath);
}

std::unique_ptr<Model> Model::Create(const std::string &name,
                                     const unsigned int processor_count,
                                     const std::string &log_name,
                                     const std::string &log_filepath) {
    return std::make_unique<Markov>(name, log_name, log_filepath,
                                    processor_count);
}
} // namespace respond
