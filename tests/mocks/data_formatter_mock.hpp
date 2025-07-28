////////////////////////////////////////////////////////////////////////////////
// File: data_formatter_mock.hpp                                              //
// Project: mocks                                                             //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_DATAFORMATTERMOCK_HPP_
#define RESPOND_TESTS_DATAFORMATTERMOCK_HPP_

#include <respond/preprocess/data_formatter.hpp>

#include <gmock/gmock.h>

#include <respond/preprocess/matrices.hpp>
#include <respond/preprocess/types.hpp>

using namespace respond::preprocess;

class MockDataFormatter : public DataFormatter {
public:
    MOCK_METHOD(void, ExtractTimesteps,
                (std::vector<int> &, History &, CostList &, TimedMatrix3d &,
                 bool),
                (override));
};

#endif // RESPOND_TESTS_DATAFORMATTERMOCK_HPP_