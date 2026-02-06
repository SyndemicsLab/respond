////////////////////////////////////////////////////////////////////////////////
// File: transition_base.hpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_INTERNALS_TRANSITION_BASE_HPP_
#define RESPOND_INTERNALS_TRANSITION_BASE_HPP_

#include <respond/transition.hpp>

namespace respond {

class TransitionBase : public virtual Transition {
public:
    TransitionBase(const std::string &name, const std::string &log_name)
        : _name(name), _log_name(log_name) {}
    virtual ~TransitionBase() = default;
    // Add a Transition Matrix to the set. We have no need to edit it once it's
    // been added, just use it. Thus, we don't need full ownership (reference)
    // and can accept the const type.
    void AddTransitionMatrix(const Eigen::MatrixXd &m) override {
        _transition_matrices.push_back(m);
    }
    // Get the name of the Transition. No need to edit the object and do not
    // need user to edit the name.
    std::string GetTransitionName() const override { return _name; }
    // Clear out all the stored Eigen::MatrixXd values
    void ClearTransitionMatrices() override { _transition_matrices.clear(); }

    std::string GetLogName() const override { return _log_name; }

protected:
    const std::vector<Eigen::MatrixXd> &GetTransitionMatrices() const {
        return _transition_matrices;
    }

private:
    std::string _name;
    std::string _log_name;
    std::vector<Eigen::MatrixXd> _transition_matrices;
};

} // namespace respond

#endif // RESPOND_INTERNALS_TRANSITION_BASE_HPP_