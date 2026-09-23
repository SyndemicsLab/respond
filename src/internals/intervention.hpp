////////////////////////////////////////////////////////////////////////////////
// File: intervention.hpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_INTERVENTION_HPP_
#define RESPOND_INTERNALS_INTERVENTION_HPP_

#include <respond/constants.hpp>

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include "transition_base.hpp"

namespace respond {
class Intervention : public virtual TransitionBase {
public:
    Intervention() : Intervention("intervention", LoggingConfig{}) {}
    Intervention(const std::string &name)
        : Intervention(name, LoggingConfig{}) {}
    [[deprecated("Use Intervention(name, LoggingConfig) instead")]]
    Intervention(const std::string &name, const std::string &log_name)
        : Intervention(name, LoggingConfig{log_name, RESPOND_DEFAULT_LOG_FILE,
                                           false}) {}
    [[deprecated("Use Intervention(name, LoggingConfig) instead")]]
    Intervention(const std::string &name, const std::string &log_name,
                 const std::string &log_file)
        : TransitionBase(name, LoggingConfig{log_name, log_file, false}) {}
    Intervention(const std::string &name,
                 const LoggingConfig &logging_config)
        : TransitionBase(name, logging_config) {}

    // Run the execute function and return the final state. Do not edit the
    // parameter state, but do edit the history provided. Nothing in the
    // Transition object should change.
    Eigen::VectorXd Execute(const Eigen::Ref<const Eigen::VectorXd> &s,
                            std::map<std::string, History> &h) const override;

    // Clone
    std::unique_ptr<Transition> clone() const override {
        auto ret = std::make_unique<Intervention>(GetName(), _logging_config);
        for (const auto &t : GetMatrices()) {
            ret->AddMatrix(t);
        }
        return ret;
    }
};
} // namespace respond

#endif // RESPOND_INTERNALS_INTERVENTION_HPP_