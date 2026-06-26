////////////////////////////////////////////////////////////////////////////////
// File: transition_mock.hpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_MARKOVMOCK_HPP_
#define RESPOND_TESTS_MARKOVMOCK_HPP_

#include <respond/transition.hpp>

#include <map>
#include <memory>
#include <vector>

#include <Eigen/Dense>
#include <gmock/gmock.h>

#include <respond/history.hpp>

namespace respond {
namespace testing {
class MockTransition : public virtual Transition {
public:
    MOCK_METHOD(Eigen::VectorXd, Execute,
                ((const Eigen::Ref<const Eigen::VectorXd> &),
                 (std::map<std::string, History> &)),
                (const, override));
    MOCK_METHOD(void, AddTransitionMatrix,
                (const Eigen::Ref<const Eigen::MatrixXd> &), (override));
    MOCK_METHOD(std::string, GetTransitionName, (), (const, override));
    MOCK_METHOD(void, ClearTransitionMatrices, (), (override));
    MOCK_METHOD(std::string, GetLogName, (), (const, override));
    MOCK_METHOD(std::unique_ptr<Transition>, clone, (), (const, override));
};
} // namespace testing
} // namespace respond

#endif // RESPOND_TESTS_MARKOVMOCK_HPP_