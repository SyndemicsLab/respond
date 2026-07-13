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

    void TestMatrixSizes(const Eigen::Ref<const Eigen::MatrixXd> &m1,
                         const Eigen::Ref<const Eigen::MatrixXd> &m2) const {
        if (m1.size() != m2.size()) {
            std::string error_msg = "Transition error - matrix size mismatch. "
                                    "Matrix 1 size is (" +
                                    std::to_string(m1.rows()) + ", " +
                                    std::to_string(m1.cols()) +
                                    ") but Matrix 2 "
                                    "size is (" +
                                    std::to_string(m2.rows()) + ", " +
                                    std::to_string(m2.cols()) + ")";
            LogError(_log_name, error_msg);
            throw std::runtime_error(error_msg);
        }
    }

    void TestSquareMatrix(const Eigen::Ref<const Eigen::MatrixXd> &m) const {
        if (m.rows() != m.cols()) {
            std::string error_msg = "Transition error - matrix is not "
                                    "square. Matrix size is (" +
                                    std::to_string(m.rows()) + ", " +
                                    std::to_string(m.cols()) + ")";
            LogError(_log_name, error_msg);
            throw std::runtime_error(error_msg);
        }
    }

    void
    TestRowColDimensions(const Eigen::Ref<const Eigen::MatrixXd> &m1,
                         const Eigen::Ref<const Eigen::MatrixXd> &m2) const {
        if (m1.rows() != m2.cols()) {
            std::stringstream ss;
            ss << "Transition error - Dimension mismatch, m1 rows do not match "
                  "m2 columns. m1 size is ("
               << m1.rows() << ", " << m1.cols() << ") m2 size is ("
               << m2.rows() << ", " << m2.cols() << ")";
            std::string error_msg = ss.str();
            LogError(_log_name, error_msg);
            throw std::runtime_error(error_msg);
        }
    }

    void
    TestColRowDimensions(const Eigen::Ref<const Eigen::MatrixXd> &m1,
                         const Eigen::Ref<const Eigen::MatrixXd> &m2) const {
        if (m1.cols() != m2.rows()) {
            std::stringstream ss;
            ss << "Transition error - Dimension mismatch, m1 columns do not "
                  "match m2 rows. m1 size is ("
               << m1.rows() << ", " << m1.cols() << ") m2 size is ("
               << m2.rows() << ", " << m2.cols() << ")";
            std::string error_msg = ss.str();
            LogError(_log_name, error_msg);
            throw std::runtime_error(error_msg);
        }
    }

    void TestCorrectNumberMatrices(const size_t &expected = 1) const {
        if (_transition_matrices.size() != expected) {
            std::string error_msg =
                "Transition error - Wrong number of matrices. Expected " +
                std::to_string(expected) + " transition matrix, got " +
                std::to_string(_transition_matrices.size());
            LogError(_log_name, error_msg);
            throw std::runtime_error(error_msg);
        }
    }

    void TestLessThanState(const Eigen::Ref<const Eigen::MatrixXd> &state,
                           const Eigen::Ref<const Eigen::MatrixXd> &m1) const {
        if (!(state.array() >= m1.array()).all()) {
            std::string error_msg =
                "Transition error - State contains values less than m1! " +
                std::to_string((state.array() < m1.array()).count()) +
                " elements affected. Verify that the transition matrix is "
                "correct and that the state vector is valid.";
            LogError(_log_name, error_msg);
            throw std::runtime_error(error_msg);
        }
    }

private:
    std::string _name;
    std::vector<Eigen::Ref<const Eigen::MatrixXd>> _transition_matrices;
};

} // namespace respond

#endif // RESPOND_INTERNALS_TRANSITION_BASE_HPP_