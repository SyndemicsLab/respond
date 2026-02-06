////////////////////////////////////////////////////////////////////////////////
// File: migration.hpp                                                        //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_MIGRATION_HPP_
#define RESPOND_INTERNALS_MIGRATION_HPP_

#include <memory>

#include "transition_base.hpp"

namespace respond {
class Migration : public virtual TransitionBase {
public:
    Migration(const std::string &name, const std::string &log_name)
        : TransitionBase(name, log_name) {}

    // Run the execute function and return the final state. Do not edit the
    // parameter state, but do edit the history provided. Nothing in the
    // Transition object should change.
    Eigen::VectorXd Execute(const Eigen::VectorXd &s,
                            std::map<std::string, History> &h) const override;

    // Clone
    std::unique_ptr<Transition> clone() const override {
        auto ret =
            std::make_unique<Migration>(GetTransitionName(), GetLogName());
        for (const auto &t : GetTransitionMatrices()) {
            ret->AddTransitionMatrix(t);
        }
        return ret;
    }

    /// @brief Factory method to create a Markov instance.
    /// @param log_name Name of the logger to write errors to.
    /// @return An instance of Markov.
    static std::unique_ptr<Transition>
    Create(const std::string &name, const std::string &log_name = "console");
};
} // namespace respond

#endif // RESPOND_INTERNALS_MIGRATION_HPP_