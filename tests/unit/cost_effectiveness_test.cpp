////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness_test.cpp                                          //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <respond/cost_effectiveness.hpp>

#include <string>
#include <vector>

#include <Eigen/Dense>
#include <gtest/gtest.h>

#include <respond/history.hpp>

namespace respond {
namespace testing {

class CostEffectivenessTest : public ::testing::Test {
protected:
    Eigen::VectorXd data;
    double discount_rate;
    int week;
    bool is_discrete;
    double total_weeks;
    void SetUp() override {
        data = Eigen::VectorXd::Constant(4, 2.0);
        discount_rate = 0.8;
        week = 1;
        is_discrete = false;
        total_weeks = 52.0;
    }
    void TearDown() override {}
};

TEST_F(CostEffectivenessTest, ContinuousDiscount) {
    double discount = exp(-discount_rate * (week / total_weeks));
    auto expected = data - Eigen::VectorXd::Constant(data.size(), discount);

    auto result = Discount(data, discount_rate, week, is_discrete, total_weeks);
    ASSERT_TRUE(expected.isApprox(result));
}

TEST_F(CostEffectivenessTest, DiscreteDiscount) {
    is_discrete = true;

    double discount = 1 / pow((1.0 + (discount_rate) / total_weeks), week);
    auto expected = data - Eigen::VectorXd::Constant(data.size(), discount);

    auto result = Discount(data, discount_rate, week, is_discrete, total_weeks);
    ASSERT_TRUE(expected.isApprox(result));
}

TEST_F(CostEffectivenessTest, CalculateLifeYearsNoHistory) {
    History h("test", "test_logger");
    double result = CalculateLifeYears(h);
    ASSERT_DOUBLE_EQ(result, 0.0);
}

TEST_F(CostEffectivenessTest, CalculateLifeYearsShortHistory) {
    History h("test", "test_logger");
    h.AddState(Eigen::VectorXd::Constant(4, 1.0), 0);
    double result = CalculateLifeYears(h, false, 0.0, 52.0);
    ASSERT_DOUBLE_EQ(result, 0.0);
}

TEST_F(CostEffectivenessTest, CalculateLifeYearsGoodHistory) {
    History h("test", "test_logger");
    h.AddState(Eigen::VectorXd::Constant(4, 1.0), 0);
    h.AddState(Eigen::VectorXd::Constant(4, 2.0), 1);
    double result = CalculateLifeYears(h, false, 0.0, 2.0);

    double expected = 12.0 / 52.0;
    ASSERT_DOUBLE_EQ(result, expected);
}

TEST_F(CostEffectivenessTest, CalculateLifeYearsDiscountHistory) {
    History h("test", "test_logger");
    auto v = Eigen::VectorXd::Constant(4, 2.0);
    h.AddState(v, 0);
    double result = CalculateLifeYears(h, true, discount_rate, 1.0);

    double discount = 1.0;
    auto discounted_result = v - Eigen::VectorXd::Constant(v.size(), discount);

    double expected = (discounted_result.sum()) / 52.0;
    ASSERT_DOUBLE_EQ(result, expected);
}

} // namespace testing
} // namespace respond
