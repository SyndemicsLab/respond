////////////////////////////////////////////////////////////////////////////////
// File: transition.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TRANSITION_HPP_
#define RESPOND_TRANSITION_HPP_

#include <functional>
#include <vector>

#include <Eigen/Dense>

#include <respond/types.hpp>

namespace respond {

/// @brief A helper class to hold Transitions
class Transition {
public:
    /// @brief A vector of matrices containing the transition matrices to
    /// multiply.
    std::vector<Eigen::MatrixXd> transition_matrices;

    void SetCallback(std::function<Eigen::VectorXd(
                         const Eigen::VectorXd &,
                         const std::vector<Eigen::MatrixXd> &, HistoryStamp &)>
                         cb) {
        _callback = std::move(cb);
    }

    Eigen::VectorXd Execute(const Eigen::VectorXd &a,
                            const std::vector<Eigen::MatrixXd> &b,
                            HistoryStamp &c) {
        if (_callback) {
            return _callback(a, b, c);
        }
        return a;
    }

private:
    /// @brief The callback function to apply.
    std::function<Eigen::VectorXd(const Eigen::VectorXd &,
                                  const std::vector<Eigen::MatrixXd> &,
                                  HistoryStamp &)>
        _callback;
};
} // namespace respond

#endif