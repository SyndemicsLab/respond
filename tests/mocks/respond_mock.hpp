////////////////////////////////////////////////////////////////////////////////
// File: respond_mock.hpp                                                     //
// Project: mocks                                                             //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TESTS_RESPONDMOCK_HPP_
#define RESPOND_TESTS_RESPONDMOCK_HPP_

#include <respond/model/markov.hpp>

#include <gmock/gmock.h>

using namespace respond::model;

class MockRespond : public Respond {
public:
    MOCK_METHOD(void, Run, (const respond::preprocess::DataLoader &),
                (override));
    MOCK_METHOD(respond::preprocess::History, GetHistory, (),
                (const, override));
};

#endif // RESPOND_TESTS_RESPONDMOCK_HPP_
