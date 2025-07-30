////////////////////////////////////////////////////////////////////////////////
// File: markov_test.cpp                                                      //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/markov.hpp>

#include <fstream>
#include <iostream>
#include <memory>

#include <gtest/gtest.h>

using namespace respond;

class MarkovTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MarkovTest, ZeroDuration) {}
