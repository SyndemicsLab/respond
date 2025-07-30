////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness_test.cpp                                          //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/cost_effectiveness.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>
#include <gtest/gtest.h>

#include <respond/types.hpp>

using namespace respond;

class CostEffectivenessTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CostEffectivenessTest, Continuous_Discount) {
    auto data = Eigen::VectorXd::Constant(4, 2.0);
    double discount_rate = 0.8;
    int week = 1;
    bool is_discrete = false;

    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 1.015266876);

    auto discounted = Discount(data, discount_rate, week, is_discrete);
    std::cout << "Discounted: " << discounted << std::endl;

    std::cout << "Expected: " << expected << std::endl;

    ASSERT_TRUE(expected.isApprox(discounted));
}

TEST_F(CostEffectivenessTest, Discrete_Discount) {
    auto data = Eigen::VectorXd::Constant(4, 2.0);
    double discount_rate = 0.8;
    int week = 1;
    bool is_discrete = true;

    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 0.984848485);

    auto discounted = Discount(data, discount_rate, week, is_discrete);

    ASSERT_TRUE(expected.isApprox(discounted));
}

TEST_F(CostEffectivenessTest, Stamp_Costs) {
    auto state = Eigen::VectorXd::Constant(4, 2.0);
    auto healthcare = Eigen::VectorXd::Constant(4, 1.23);
    auto aod = Eigen::VectorXd::Constant(4, 12.34);
    auto fod = Eigen::VectorXd::Constant(4, 123.45);
    auto pharma = Eigen::VectorXd::Constant(4, 1234.56);
    auto treatment = Eigen::VectorXd::Constant(4, 12345.67);

    CostStamp expected;
    expected.healthcare = state.cwiseProduct(healthcare);
    expected.non_fatal_overdoses = state.cwiseProduct(aod);
    expected.fatal_overdoses = state.cwiseProduct(fod);
    expected.pharmaceuticals = state.cwiseProduct(pharma);
    expected.treatments = state.cwiseProduct(treatment);

    auto result = StampCosts(state, healthcare, aod, fod, pharma, treatment);

    ASSERT_TRUE(expected.healthcare.isApprox(result.healthcare));
    ASSERT_TRUE(
        expected.non_fatal_overdoses.isApprox(result.non_fatal_overdoses));
    ASSERT_TRUE(expected.fatal_overdoses.isApprox(result.fatal_overdoses));
    ASSERT_TRUE(expected.pharmaceuticals.isApprox(result.pharmaceuticals));
    ASSERT_TRUE(expected.treatments.isApprox(result.treatments));
}

TEST_F(CostEffectivenessTest, Stamp_Utilities) {
    auto state = Eigen::VectorXd::Constant(4, 2.0);
    auto utility = Eigen::VectorXd::Constant(4, 0.72);
    Eigen::VectorXd expected = state.cwiseProduct(utility);
    auto result = StampUtilities(state, utility);
    ASSERT_TRUE(expected.isApprox(result));
}

TEST_F(CostEffectivenessTest, StampUtilities_InvalidSize) {
    auto state = Eigen::VectorXd::Constant(4, 2.0);
    auto utility = Eigen::VectorXd::Constant(1, 0.72);
    Eigen::VectorXd expected = Eigen::VectorXd::Ones(state.size());
    auto result = StampUtilities(state, utility);
    ASSERT_TRUE(expected.isApprox(result));
}

TEST_F(CostEffectivenessTest, StampCostsOverTime_NoDiscount) {
    HistoryOverTime history_over_time;
    history_over_time[0] = HistoryStamp{Eigen::VectorXd::Constant(4, 3.0),
                                        Eigen::VectorXd::Constant(4, 2.0),
                                        Eigen::VectorXd::Constant(4, 1.0)};
    history_over_time[1] = HistoryStamp{Eigen::VectorXd::Constant(4, 4.0),
                                        Eigen::VectorXd::Constant(4, 3.0),
                                        Eigen::VectorXd::Constant(4, 2.0)};
    history_over_time[2] = HistoryStamp{Eigen::VectorXd::Constant(4, 5.0),
                                        Eigen::VectorXd::Constant(4, 4.0),
                                        Eigen::VectorXd::Constant(4, 3.0)};

    auto healthcare = Eigen::VectorXd::Constant(4, 1.23);
    auto aod = Eigen::VectorXd::Constant(4, 12.34);
    auto fod = Eigen::VectorXd::Constant(4, 123.45);
    auto pharma = Eigen::VectorXd::Constant(4, 1234.56);
    auto treatment = Eigen::VectorXd::Constant(4, 12345.67);

    CostsOverTime expected;
    expected[0] = CostStamp{Eigen::VectorXd::Constant(4, 3.69),
                            Eigen::VectorXd::Constant(4, 37.02),
                            Eigen::VectorXd::Constant(4, 370.35),
                            Eigen::VectorXd::Constant(4, 3703.68),
                            Eigen::VectorXd::Constant(4, 37037.01)};

    expected[1] = CostStamp{Eigen::VectorXd::Constant(4, 4.92),
                            Eigen::VectorXd::Constant(4, 49.36),
                            Eigen::VectorXd::Constant(4, 493.80),
                            Eigen::VectorXd::Constant(4, 4938.24),
                            Eigen::VectorXd::Constant(4, 49382.68)};

    expected[2] = CostStamp{Eigen::VectorXd::Constant(4, 6.15),
                            Eigen::VectorXd::Constant(4, 61.70),
                            Eigen::VectorXd::Constant(4, 617.25),
                            Eigen::VectorXd::Constant(4, 6172.80),
                            Eigen::VectorXd::Constant(4, 61728.35)};

    auto result = StampCostsOverTime(history_over_time, healthcare, aod, fod,
                                     pharma, treatment);

    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(expected[i].healthcare.isApprox(result[i].healthcare));
        ASSERT_TRUE(expected[i].non_fatal_overdoses.isApprox(
            result[i].non_fatal_overdoses));
        ASSERT_TRUE(
            expected[i].fatal_overdoses.isApprox(result[i].fatal_overdoses));
        ASSERT_TRUE(
            expected[i].pharmaceuticals.isApprox(result[i].pharmaceuticals));
        ASSERT_TRUE(expected[i].treatments.isApprox(result[i].treatments));
    }
}

TEST_F(CostEffectivenessTest, StampCostsOverTime_Discount) {
    HistoryOverTime history_over_time;
    history_over_time[0] = HistoryStamp{Eigen::VectorXd::Constant(4, 3.0),
                                        Eigen::VectorXd::Constant(4, 2.0),
                                        Eigen::VectorXd::Constant(4, 1.0)};
    history_over_time[1] = HistoryStamp{Eigen::VectorXd::Constant(4, 4.0),
                                        Eigen::VectorXd::Constant(4, 3.0),
                                        Eigen::VectorXd::Constant(4, 2.0)};
    history_over_time[2] = HistoryStamp{Eigen::VectorXd::Constant(4, 5.0),
                                        Eigen::VectorXd::Constant(4, 4.0),
                                        Eigen::VectorXd::Constant(4, 3.0)};

    auto healthcare = Eigen::VectorXd::Constant(4, 1.23);
    auto aod = Eigen::VectorXd::Constant(4, 12.34);
    auto fod = Eigen::VectorXd::Constant(4, 123.45);
    auto pharma = Eigen::VectorXd::Constant(4, 1234.56);
    auto treatment = Eigen::VectorXd::Constant(4, 12345.67);

    CostsOverTime expected;
    expected[0] = CostStamp{Eigen::VectorXd::Constant(4, 3.69),
                            Eigen::VectorXd::Constant(4, 37.02),
                            Eigen::VectorXd::Constant(4, 370.35),
                            Eigen::VectorXd::Constant(4, 3703.68),
                            Eigen::VectorXd::Constant(4, 37037.01)};

    expected[1] = CostStamp{Eigen::VectorXd::Constant(4, 4.92),
                            Eigen::VectorXd::Constant(4, 49.36),
                            Eigen::VectorXd::Constant(4, 493.80),
                            Eigen::VectorXd::Constant(4, 4938.24),
                            Eigen::VectorXd::Constant(4, 49382.68)};

    expected[2] = CostStamp{Eigen::VectorXd::Constant(4, 6.15),
                            Eigen::VectorXd::Constant(4, 61.70),
                            Eigen::VectorXd::Constant(4, 617.25),
                            Eigen::VectorXd::Constant(4, 6172.80),
                            Eigen::VectorXd::Constant(4, 61728.35)};

    auto result = StampCostsOverTime(history_over_time, healthcare, aod, fod,
                                     pharma, treatment, true, 0.8);

    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(expected[i].healthcare.isApprox(result[i].healthcare));
        ASSERT_TRUE(expected[i].non_fatal_overdoses.isApprox(
            result[i].non_fatal_overdoses));
        ASSERT_TRUE(
            expected[i].fatal_overdoses.isApprox(result[i].fatal_overdoses));
        ASSERT_TRUE(
            expected[i].pharmaceuticals.isApprox(result[i].pharmaceuticals));
        ASSERT_TRUE(expected[i].treatments.isApprox(result[i].treatments));
    }
}

TEST_F(CostEffectivenessTest, CalculateTotalCosts) {}
