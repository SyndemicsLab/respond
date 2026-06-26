////////////////////////////////////////////////////////////////////////////////
// File: model_mock.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2025-08-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_MODELMOCK_HPP_
#define RESPOND_TESTS_MODELMOCK_HPP_

#include <respond/history.hpp>
#include <respond/model.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <respond/transition.hpp>

namespace respond {
namespace testing {
class MockModel : public virtual Model {
public:
    MOCK_METHOD(void, SetState, (const Eigen::Ref<const Eigen::VectorXd> &),
                (override));
    MOCK_METHOD(Eigen::VectorXd, GetState, (), (const, override));
    MOCK_METHOD(void, RunTransitions, (), (override));
    MOCK_METHOD(void, AddTransition, (const std::unique_ptr<Transition> &),
                (override));
    MOCK_METHOD((std::vector<std::string>), GetTransitionNames, (),
                (const, override));
    MOCK_METHOD(void, ClearTransitions, (), (override));
    MOCK_METHOD((std::map<std::string, History>), GetHistories, (),
                (const, override));
    MOCK_METHOD(void, SetHistories, ((const std::map<std::string, History> &)),
                (override));
    MOCK_METHOD(void, ClearHistories, (), (override));
    MOCK_METHOD(void, SetHistoryCaptureInterval, (int), (override));
    MOCK_METHOD(int, GetHistoryCaptureInterval, (), (const, override));
    MOCK_METHOD(void, SetFinalTimestep, (int), (override));
    MOCK_METHOD(int, GetFinalTimestep, (), (const, override));
    MOCK_METHOD(std::string, GetModelName, (), (const, override));
    MOCK_METHOD(std::string, GetLogName, (), (const, override));
    MOCK_METHOD((std::unique_ptr<Model>), clone, (), (const, override));
    MOCK_METHOD(void, CreateDefaultHistories, (), (override));
};
} // namespace testing
} // namespace respond

#endif // RESPOND_TESTS_MODELMOCK_HPP_