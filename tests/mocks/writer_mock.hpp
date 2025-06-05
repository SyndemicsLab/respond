////////////////////////////////////////////////////////////////////////////////
// File: writer_mock.hpp                                                      //
// Project: mocks                                                             //
// Created Date: 2025-03-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_WRITERMOCK_HPP_
#define RESPOND_TESTS_WRITERMOCK_HPP_

#include <respond/data_ops/writer.hpp>

#include <string>

#include <gmock/gmock.h>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

using namespace respond::data_ops;

class MockWriter : public Writer {
public:
    MOCK_METHOD(std::string, WriteInputData,
                (const DataLoader &, const std::string &, const OutputType),
                (const, override));
    MOCK_METHOD(std::string, WriteHistoryData,
                (const History &, const std::string &, const OutputType),
                (const, override));
    MOCK_METHOD(std::string, WriteCostData,
                (const CostList &, const std::string &, const OutputType),
                (const, override));
    MOCK_METHOD(std::string, WriteUtilityData,
                (const TimedMatrix3d &, const std::string &, const OutputType),
                (const, override));
    MOCK_METHOD(std::string, WriteTotalsData,
                (const Totals &, const std::string &, const OutputType),
                (const, override));
};

#endif // RESPOND_TESTS_WRITERMOCK_HPP_