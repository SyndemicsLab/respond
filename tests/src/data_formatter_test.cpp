////////////////////////////////////////////////////////////////////////////////
// File: data_formatter_test.cpp                                              //
// Project: src                                                               //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/preprocess/data_formatter.hpp>

#include <gtest/gtest.h>

using namespace respond::preprocess;

class DataFormatterTest : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(DataFormatterTest, ExtractTimesteps_Single_Timestep) {
    // Write the Test
}