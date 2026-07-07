////////////////////////////////////////////////////////////////////////////////
// File: background.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_BACKGROUND_HPP_
#define RESPOND_INTERNALS_BACKGROUND_HPP_

#include <memory>

#include "transition_base.hpp"

namespace respond {
class BackgroundDeath : public virtual TransitionBase {
public:
    BackgroundDeath() : BackgroundDeath("background_death") {}
    BackgroundDeath(const std::string &name)
        : BackgroundDeath(name, RESPOND_DEFAULT_LOG) {}
    BackgroundDeath(const std::string &name, const std::string &log_name)
        : BackgroundDeath(name, log_name, RESPOND_DEFAULT_LOG_FILE) {}
    BackgroundDeath(const std::string &name, const std::string &log_name,
                    const std::string &log_file)
        : TransitionBase(name, log_name, log_file) {}

    // Run the execute function and return the final state. Do not edit the
    // parameter state, but do edit the history provided. Nothing in the
    // Transition object should change.
    Eigen::VectorXd Execute(const Eigen::Ref<const Eigen::VectorXd> &s,
                            std::map<std::string, History> &h) const override;

    // Clone
    std::unique_ptr<Transition> clone() const override {
        auto ret = std::make_unique<BackgroundDeath>(GetName(), _log_name);
        for (const auto &t : GetMatrices()) {
            ret->AddMatrix(t);
        }
        return ret;
    }
};
} // namespace respond

#endif // RESPOND_INTERNALS_BACKGROUND_HPP_