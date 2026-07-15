////////////////////////////////////////////////////////////////////////////////
// File: migration.hpp                                                        //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_MIGRATION_HPP_
#define RESPOND_INTERNALS_MIGRATION_HPP_

#include <respond/constants.hpp>

#include <map>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include "transition_base.hpp"

namespace respond {
class Migration : public virtual TransitionBase {
public:
    Migration() : Migration("migration") {}
    Migration(const std::string &name) : Migration(name, RESPOND_DEFAULT_LOG) {}
    Migration(const std::string &name, const std::string &log_name)
        : Migration(name, log_name, RESPOND_DEFAULT_LOG_FILE) {}
    Migration(const std::string &name, const std::string &log_name,
              const std::string &log_file)
        : TransitionBase(name, log_name, log_file) {}

    // Run the execute function and return the final state. Do not edit the
    // parameter state, but do edit the history provided. Nothing in the
    // Transition object should change.
    Eigen::VectorXd Execute(const Eigen::Ref<const Eigen::VectorXd> &s,
                            std::map<std::string, History> &h) const override;

    // Clone
    std::unique_ptr<Transition> clone() const override {
        auto ret = std::make_unique<Migration>(GetName(), _log_name);
        for (const auto &t : GetMatrices()) {
            ret->AddMatrix(t);
        }
        return ret;
    }
};
} // namespace respond

#endif // RESPOND_INTERNALS_MIGRATION_HPP_