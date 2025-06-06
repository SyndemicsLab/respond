////////////////////////////////////////////////////////////////////////////////
// File: data_formatter_mock.hpp                                              //
// Project: mocks                                                             //
// Created Date: 2025-03-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_DATAFORMATTERMOCK_HPP_
#define RESPOND_TESTS_DATAFORMATTERMOCK_HPP_

#include <respond/data_ops/data_formatter.hpp>

#include <gmock/gmock.h>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

using namespace respond::data_ops;

class MockDataFormatter : public DataFormatter {
public:
    MOCK_METHOD(void, ExtractTimesteps,
                (std::vector<int> &, History &, CostList &, TimedMatrix3d &,
                 bool),
                (override));
};

#endif // RESPOND_TESTS_DATAFORMATTERMOCK_HPP_