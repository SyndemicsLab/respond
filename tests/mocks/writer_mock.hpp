////////////////////////////////////////////////////////////////////////////////
// File: writer_mock.hpp                                                      //
// Project: mocks                                                             //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_WRITERMOCK_HPP_
#define RESPOND_TESTS_WRITERMOCK_HPP_

#include <respond/postprocess/writer.hpp>

#include <string>

#include <gmock/gmock.h>

#include <respond/preprocess/data_types.hpp>
#include <respond/preprocess/matrices.hpp>

using namespace respond::preprocess;

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