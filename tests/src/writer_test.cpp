////////////////////////////////////////////////////////////////////////////////
// File: TEST_Writer.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/writer.hpp>

#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <unsupported/Eigen/CXX11/Tensor>

using namespace respond::data_ops;

class WriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};
