////////////////////////////////////////////////////////////////////////////////
// File: respond_mock.hpp                                                     //
// Project: mocks                                                             //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TESTS_RESPONDMOCK_HPP_
#define RESPOND_TESTS_RESPONDMOCK_HPP_

#include <respond/model/simulation.hpp>

#include <gmock/gmock.h>

using namespace respond::model;

class MockRespond : public Respond {
public:
    MOCK_METHOD(void, Run, (const respond::data_ops::DataLoader &), (override));
    MOCK_METHOD(respond::data_ops::History, GetHistory, (), (const, override));
};

#endif // RESPOND_TESTS_RESPONDMOCK_HPP_
