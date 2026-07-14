////////////////////////////////////////////////////////////////////////////////
// File: transition_mock.hpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_MARKOVMOCK_HPP_
#define RESPOND_TESTS_MARKOVMOCK_HPP_

#include <respond/history.hpp>
#include <respond/transition.hpp>

#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include <Eigen/Dense>
#include <gmock/gmock.h>

namespace respond {
namespace testing {
class MockTransition : public virtual Transition {
public:
    MOCK_METHOD(Eigen::VectorXd, Execute,
                ((const Eigen::Ref<const Eigen::VectorXd> &),
                 (std::map<std::string, History> &)),
                (const, override));
    MOCK_METHOD(void, AddMatrix, (Eigen::Ref<const Eigen::MatrixXd>),
                (override));
    MOCK_METHOD((std::vector<Eigen::MatrixXd>), GetMatrices, (),
                (const, override));
    MOCK_METHOD(void, ClearMatrices, (), (override));
    MOCK_METHOD(std::string, GetName, (), (const, override));
    MOCK_METHOD(std::unique_ptr<Transition>, clone, (), (const, override));
    MOCK_METHOD(void, Serialize, (std::ostream &), (const, override));
};
} // namespace testing
} // namespace respond

#endif // RESPOND_TESTS_MARKOVMOCK_HPP_