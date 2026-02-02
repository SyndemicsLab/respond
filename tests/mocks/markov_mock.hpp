////////////////////////////////////////////////////////////////////////////////
// File: markov_mock.hpp                                                      //
// Project: respond                                                           //
// Created Date: 2025-08-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_MARKOVMOCK_HPP_
#define RESPOND_TESTS_MARKOVMOCK_HPP_

#include <respond/markov.hpp>

#include <vector>

#include <Eigen/Dense>
#include <gmock/gmock.h>

#include <respond/transition.hpp>

namespace respond {
class MockMarkov : public virtual Markov {
public:
    MOCK_METHOD(void, SetState, (const Eigen::VectorXd &), (override));
    MOCK_METHOD(Eigen::VectorXd, GetState, (), (const, override));
    MOCK_METHOD(void, SetTransitions, (const std::vector<Transition> &),
                (override));
    MOCK_METHOD((std::vector<Transition>), GetTransitions, (),
                (const, override));
    MOCK_METHOD(void, AddTransition, (const Transition &), (override));
    MOCK_METHOD(void, Run, (const int &), (override));
    MOCK_METHOD(HistoryOverTime, GetRunResults, (), (const, override));
};
} // namespace respond

#endif // RESPOND_TESTS_MARKOVMOCK_HPP_