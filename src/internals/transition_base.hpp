////////////////////////////////////////////////////////////////////////////////
// File: transition_base.hpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_TRANSITION_BASE_HPP_
#define RESPOND_INTERNALS_TRANSITION_BASE_HPP_

#include <respond/logging.hpp>
#include <respond/transition.hpp>

#include <string>
#include <vector>

#include <Eigen/Dense>

namespace respond {

class TransitionBase : public virtual Transition {
public:
    TransitionBase(const std::string &name, const std::string &log_name,
                   const std::string &log_file)
        : _name(name), _log_name(log_name) {
        CreateFileLogger(log_name, log_file);
    }
    virtual ~TransitionBase() = default;
    // Add a Transition Matrix to the set. We have no need to edit it once it's
    // been added, just use it. Thus, we don't need full ownership (reference)
    // and can accept the const type.
    void AddMatrix(const Eigen::Ref<const Eigen::MatrixXd> &m) override {
        _transition_matrices.push_back(m);
    }
    std::vector<Eigen::Ref<const Eigen::MatrixXd>>
    GetMatrices() const override {
        return _transition_matrices;
    }
    // Get the name of the Transition. No need to edit the object and do not
    // need user to edit the name.
    std::string GetName() const override { return _name; }
    // Clear out all the stored Eigen::MatrixXd values
    void ClearMatrices() override { _transition_matrices.clear(); }

protected:
    const std::string _log_name;

private:
    std::string _name;
    std::vector<Eigen::Ref<const Eigen::MatrixXd>> _transition_matrices;
};

} // namespace respond

#endif // RESPOND_INTERNALS_TRANSITION_BASE_HPP_