////////////////////////////////////////////////////////////////////////////////
// File: cost_effectiveness_test.cpp                                          //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-31                                                  //
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
    HistoryOverTime history_over_time;

    Eigen::VectorXd healthcare;
    Eigen::VectorXd aod;
    Eigen::VectorXd fod;
    Eigen::VectorXd pharma;
    Eigen::VectorXd treatment;
    Eigen::VectorXd utility;
    double accuracy = 1e-6;

    void SetUp() override {
        history_over_time[0] = HistoryStamp{Eigen::VectorXd::Constant(4, 3.0),
                                            Eigen::VectorXd::Constant(4, 2.0),
                                            Eigen::VectorXd::Constant(4, 1.0)};
        history_over_time[1] = HistoryStamp{Eigen::VectorXd::Constant(4, 4.0),
                                            Eigen::VectorXd::Constant(4, 3.0),
                                            Eigen::VectorXd::Constant(4, 2.0)};
        history_over_time[2] = HistoryStamp{Eigen::VectorXd::Constant(4, 5.0),
                                            Eigen::VectorXd::Constant(4, 4.0),
                                            Eigen::VectorXd::Constant(4, 3.0)};
        healthcare = Eigen::VectorXd::Constant(4, 1.23);
        aod = Eigen::VectorXd::Constant(4, 12.34);
        fod = Eigen::VectorXd::Constant(4, 123.45);
        pharma = Eigen::VectorXd::Constant(4, 1234.56);
        treatment = Eigen::VectorXd::Constant(4, 12345.67);
        utility = Eigen::VectorXd::Constant(4, 0.72);
    }
    void TearDown() override {}
};

TEST_F(CostEffectivenessTest, Continuous_Discount) {
    auto data = Eigen::VectorXd::Constant(4, 2.0);
    double discount_rate = 0.8;
    int week = 1;
    bool is_discrete = false;

    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 1.015266876);

    auto discounted = Discount(data, discount_rate, week, is_discrete);
    ASSERT_TRUE(expected.isApprox(discounted, accuracy));
}

TEST_F(CostEffectivenessTest, Discrete_Discount) {
    auto data = Eigen::VectorXd::Constant(4, 2.0);
    double discount_rate = 0.8;
    int week = 1;
    bool is_discrete = true;

    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 1.015151515);

    auto discounted = Discount(data, discount_rate, week, is_discrete);

    ASSERT_TRUE(expected.isApprox(discounted, accuracy));
}

TEST_F(CostEffectivenessTest, Stamp_Costs) {
    auto state = Eigen::VectorXd::Constant(4, 2.0);

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
    Eigen::VectorXd expected = state.cwiseProduct(utility);
    auto result = StampUtilities(state, utility);
    ASSERT_TRUE(expected.isApprox(result));
}

TEST_F(CostEffectivenessTest, StampUtilities_InvalidSize) {
    auto state = Eigen::VectorXd::Constant(4, 2.0);
    utility = Eigen::VectorXd::Constant(5, 0.72); // Different size
    Eigen::VectorXd expected = Eigen::VectorXd::Ones(state.size());
    auto result = StampUtilities(state, utility);
    ASSERT_TRUE(expected.isApprox(result));
}

TEST_F(CostEffectivenessTest, StampCostsOverTime_NoDiscount) {
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
    CostsOverTime expected;
    expected[0] = CostStamp{Eigen::VectorXd::Constant(4, 3.69 - 1.0),
                            Eigen::VectorXd::Constant(4, 37.02 - 1.0),
                            Eigen::VectorXd::Constant(4, 370.35 - 1.0),
                            Eigen::VectorXd::Constant(4, 3703.68 - 1.0),
                            Eigen::VectorXd::Constant(4, 37037.01 - 1.0)};

    expected[1] =
        CostStamp{Eigen::VectorXd::Constant(4, 4.92 - 0.984848485),
                  Eigen::VectorXd::Constant(4, 49.36 - 0.984848485),
                  Eigen::VectorXd::Constant(4, 493.80 - 0.984848485),
                  Eigen::VectorXd::Constant(4, 4938.24 - 0.984848485),
                  Eigen::VectorXd::Constant(4, 49382.68 - 0.984848485)};

    expected[2] =
        CostStamp{Eigen::VectorXd::Constant(4, 6.15 - 0.969926538),
                  Eigen::VectorXd::Constant(4, 61.70 - 0.969926538),
                  Eigen::VectorXd::Constant(4, 617.25 - 0.969926538),
                  Eigen::VectorXd::Constant(4, 6172.80 - 0.969926538),
                  Eigen::VectorXd::Constant(4, 61728.35 - 0.969926538)};

    auto result = StampCostsOverTime(history_over_time, healthcare, aod, fod,
                                     pharma, treatment, true, 0.8);

    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(
            expected[i].healthcare.isApprox(result[i].healthcare, accuracy));
        ASSERT_TRUE(expected[i].non_fatal_overdoses.isApprox(
            result[i].non_fatal_overdoses, accuracy));
        ASSERT_TRUE(expected[i].fatal_overdoses.isApprox(
            result[i].fatal_overdoses, accuracy));
        ASSERT_TRUE(expected[i].pharmaceuticals.isApprox(
            result[i].pharmaceuticals, accuracy));
        ASSERT_TRUE(
            expected[i].treatments.isApprox(result[i].treatments, accuracy));
    }
}

TEST_F(CostEffectivenessTest, StampUtilitiesOverTime_Min_NoDiscount) {
    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 2.16);
    auto result =
        StampUtilitiesOverTime(history_over_time, utility, UtilityType::kMin);
    ASSERT_TRUE(expected.isApprox(result, accuracy));
}

TEST_F(CostEffectivenessTest, StampUtilitiesOverTime_Mult_NoDiscount) {
    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 48.3729);
    auto result =
        StampUtilitiesOverTime(history_over_time, utility, UtilityType::kMult);
    ASSERT_TRUE(expected.isApprox(result, accuracy));
}

TEST_F(CostEffectivenessTest, StampUtilitiesOverTime_Min_Discount) {
    Eigen::VectorXd expected = Eigen::VectorXd::Constant(4, 1.16);
    auto result = StampUtilitiesOverTime(history_over_time, utility,
                                         UtilityType::kMin, true, 0.8);
    ASSERT_TRUE(expected.isApprox(result, accuracy));
}

TEST_F(CostEffectivenessTest, CalculatePerspectives_NoDiscount) {
    CostsOverTime expected_one;
    expected_one[0] = CostStamp{Eigen::VectorXd::Constant(4, 3.69),
                                Eigen::VectorXd::Constant(4, 37.02),
                                Eigen::VectorXd::Constant(4, 370.35),
                                Eigen::VectorXd::Constant(4, 3703.68),
                                Eigen::VectorXd::Constant(4, 37037.01)};

    expected_one[1] = CostStamp{Eigen::VectorXd::Constant(4, 4.92),
                                Eigen::VectorXd::Constant(4, 49.36),
                                Eigen::VectorXd::Constant(4, 493.80),
                                Eigen::VectorXd::Constant(4, 4938.24),
                                Eigen::VectorXd::Constant(4, 49382.68)};

    expected_one[2] = CostStamp{Eigen::VectorXd::Constant(4, 6.15),
                                Eigen::VectorXd::Constant(4, 61.70),
                                Eigen::VectorXd::Constant(4, 617.25),
                                Eigen::VectorXd::Constant(4, 6172.80),
                                Eigen::VectorXd::Constant(4, 61728.35)};

    CostsOverTime expected_two;
    expected_two[0] = CostStamp{
        Eigen::VectorXd::Constant(4, 3.0), Eigen::VectorXd::Constant(4, 3.0),
        Eigen::VectorXd::Constant(4, 3.0), Eigen::VectorXd::Constant(4, 3.0),
        Eigen::VectorXd::Constant(4, 3.0)};
    expected_two[1] = CostStamp{
        Eigen::VectorXd::Constant(4, 4.0), Eigen::VectorXd::Constant(4, 4.0),
        Eigen::VectorXd::Constant(4, 4.0), Eigen::VectorXd::Constant(4, 4.0),
        Eigen::VectorXd::Constant(4, 4.0)};
    expected_two[2] = CostStamp{
        Eigen::VectorXd::Constant(4, 5.0), Eigen::VectorXd::Constant(4, 5.0),
        Eigen::VectorXd::Constant(4, 5.0), Eigen::VectorXd::Constant(4, 5.0),
        Eigen::VectorXd::Constant(4, 5.0)};

    std::vector<Eigen::VectorXd> healthcare_costs = {
        healthcare, Eigen::VectorXd::Constant(4, 1.00)};

    std::vector<Eigen::VectorXd> aod_costs = {
        aod, Eigen::VectorXd::Constant(4, 1.00)};

    std::vector<Eigen::VectorXd> fod_costs = {
        fod, Eigen::VectorXd::Constant(4, 1.00)};

    std::vector<Eigen::VectorXd> pharma_costs = {
        pharma, Eigen::VectorXd::Constant(4, 1.00)};

    std::vector<Eigen::VectorXd> treatment_costs = {
        treatment, Eigen::VectorXd::Constant(4, 1.00)};

    auto result = CalculatePerspectives(
        history_over_time, {"Perspective1", "Perspective2"}, healthcare_costs,
        aod_costs, fod_costs, pharma_costs, treatment_costs);

    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(expected_one[i].healthcare.isApprox(
            result["Perspective1"][i].healthcare));
        ASSERT_TRUE(expected_one[i].non_fatal_overdoses.isApprox(
            result["Perspective1"][i].non_fatal_overdoses));
        ASSERT_TRUE(expected_one[i].fatal_overdoses.isApprox(
            result["Perspective1"][i].fatal_overdoses));
        ASSERT_TRUE(expected_one[i].pharmaceuticals.isApprox(
            result["Perspective1"][i].pharmaceuticals));
        ASSERT_TRUE(expected_one[i].treatments.isApprox(
            result["Perspective1"][i].treatments));
    }

    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(expected_two[i].healthcare.isApprox(
            result["Perspective2"][i].healthcare));
        ASSERT_TRUE(expected_two[i].non_fatal_overdoses.isApprox(
            result["Perspective2"][i].non_fatal_overdoses));
        ASSERT_TRUE(expected_two[i].fatal_overdoses.isApprox(
            result["Perspective2"][i].fatal_overdoses));
        ASSERT_TRUE(expected_two[i].pharmaceuticals.isApprox(
            result["Perspective2"][i].pharmaceuticals));
        ASSERT_TRUE(expected_two[i].treatments.isApprox(
            result["Perspective2"][i].treatments));
    }
}

TEST_F(CostEffectivenessTest, CalculateLifeYears_NoHistory) {
    HistoryOverTime empty_history;
    double result = CalculateLifeYears(empty_history);
    ASSERT_DOUBLE_EQ(result, 0.0);
}

TEST_F(CostEffectivenessTest, CalculateLifeYears_ShortHistory) {
    HistoryOverTime short_history = {
        {0, HistoryStamp{Eigen::VectorXd::Constant(4, 1.0),
                         Eigen::VectorXd::Constant(4, 1.0),
                         Eigen::VectorXd::Constant(4, 1.0)}}};
    double result = CalculateLifeYears(short_history);
    ASSERT_DOUBLE_EQ(result, 0.0);
}

TEST_F(CostEffectivenessTest, CalculateLifeYears_GoodHistory) {
    HistoryOverTime history;
    for (int i = 0; i < 52; ++i) {
        history[i] = HistoryStamp{Eigen::VectorXd::Constant(4, 1.0),
                                  Eigen::VectorXd::Constant(4, 1.0),
                                  Eigen::VectorXd::Constant(4, 1.0)};
    }
    double result = CalculateLifeYears(history);
    ASSERT_DOUBLE_EQ(result, 4.0);
}

TEST_F(CostEffectivenessTest, CalculateLifeYears_DiscountHistory) {
    HistoryOverTime history;
    for (int i = 0; i < 52; ++i) {
        history[i] = HistoryStamp{Eigen::VectorXd::Constant(4, 1.0),
                                  Eigen::VectorXd::Constant(4, 1.0),
                                  Eigen::VectorXd::Constant(4, 1.0)};
    }
    double result = CalculateLifeYears(history, true, 0.8);
    ASSERT_DOUBLE_EQ(result, 1.2182237854026694);
}

TEST_F(CostEffectivenessTest, CalculateTotalCosts_Full) {
    CostsOverTime costs;
    costs[0] = CostStamp{
        Eigen::VectorXd::Constant(4, 1.0), Eigen::VectorXd::Constant(4, 2.0),
        Eigen::VectorXd::Constant(4, 3.0), Eigen::VectorXd::Constant(4, 4.0),
        Eigen::VectorXd::Constant(4, 5.0)};

    costs[1] = CostStamp{
        Eigen::VectorXd::Constant(4, 2.0), Eigen::VectorXd::Constant(4, 2.0),
        Eigen::VectorXd::Constant(4, 3.0), Eigen::VectorXd::Constant(4, 4.0),
        Eigen::VectorXd::Constant(4, 5.0)};

    costs[2] = CostStamp{
        Eigen::VectorXd::Constant(4, 3.0), Eigen::VectorXd::Constant(4, 2.0),
        Eigen::VectorXd::Constant(4, 3.0), Eigen::VectorXd::Constant(4, 4.0),
        Eigen::VectorXd::Constant(4, 5.0)};

    std::vector<double> expected = {60.0, 64.0, 68.0};

    auto result = CalculateTotalCosts(costs);
    for (int i = 0; i < expected.size(); ++i) {
        ASSERT_DOUBLE_EQ(expected[i], result[i]);
    }
}
