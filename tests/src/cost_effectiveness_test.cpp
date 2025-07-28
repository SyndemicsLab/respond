////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness_test.cpp                                          //
// Project: src                                                               //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/model/post_sim.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <respond/preprocess/types.hpp>

using namespace respond::model;
using namespace respond::preprocess;

class PostSimTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PostSimTest, CalculateContinuousDiscount) {
    Matrix3d temp(1, 1, 1);
    temp.setConstant(2.0);
    auto discounted = CalculateDiscount(temp, 0.8, 1, false);

    Matrix3d expected(1, 1, 1);
    expected.setConstant(1.015266876);

    Eigen::Tensor<bool, 0> eq =
        ((expected - discounted).abs() > 0.000001).all();

    ASSERT_TRUE(eq(0));
}

TEST_F(PostSimTest, CalculateDiscreteDiscount) {
    Matrix3d temp(1, 1, 1);
    temp.setConstant(2.0);
    auto discounted = CalculateDiscount(temp, 0.8, 1, true);

    Matrix3d expected(1, 1, 1);
    expected.setConstant(1.018867924);

    Eigen::Tensor<bool, 0> eq =
        ((expected - discounted).abs() > 0.000001).all();

    ASSERT_TRUE(eq(0));
}

TEST_F(PostSimTest, CalculateCosts) {}

TEST_F(PostSimTest, CalculateUtilities) {}

TEST_F(PostSimTest, CalculateLifeYears) {}

TEST_F(PostSimTest, CalculateTotalCosts) {}
