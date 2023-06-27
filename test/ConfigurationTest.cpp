//===-- ConfigurationTest.cpp - Instruction class definition ----*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:33 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <gtest/gtest.h>

#include "Configuration.hpp"
#include "DataTypes.hpp"

using namespace Data;

TEST(ConfigurationTest, EmptyConstructor) {
    Configuration config;
    EXPECT_TRUE(&config != nullptr);
}

TEST(ConfigurationTest, GeneralConstructor) {
    Configuration config("TestInput/input1/sim.conf");
    EXPECT_TRUE(&config != nullptr);
}

TEST(ConfigurationTest, getInterventions) {
    std::vector<std::string> EXPECTED = {"No_Treatment", "Buprenorphine",
                                         "Naltrexone", "Post-Buprenorphine",
                                         "Post-Naltrexone"};

    Configuration config("TestInput/input1/sim.conf");

    std::vector<std::string> ACTUAL = config.getInterventions();
    for (int i = 0; i < ACTUAL.size(); i++) {
        EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    }
}

TEST(ConfigurationTest, getOUDStates) {
    std::vector<std::string> EXPECTED = {
        "Active_Noninjection", "Active_Injection", "Nonactive_Noninjection",
        "Nonactive_Injection"};

    Configuration config("TestInput/input1/sim.conf");

    std::vector<std::string> ACTUAL = config.getOUDStates();
    for (int i = 0; i < ACTUAL.size(); i++) {
        EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    }
}

TEST(ConfigurationTest, getNumDemographicCombos) {
    int EXPECTED = 6;

    Configuration config("TestInput/input1/sim.conf");

    int ACTUAL = config.getNumDemographicCombos();
    EXPECT_EQ(EXPECTED, ACTUAL);
}

TEST(ConfigurationTest, getEnteringSampleChangeTimes) {
    std::vector<int> EXPECTED = {51, 103, 155};

    Configuration config("TestInput/input1/sim.conf");

    std::vector<int> ACTUAL = config.getEnteringSampleChangeTimes();
    for (int i = 0; i < ACTUAL.size(); i++) {
        EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    }
}

TEST(ConfigurationTest, getInterventionChangeTimes) {
    std::vector<int> EXPECTED = {51, 103, 155};

    Configuration config("TestInput/input1/sim.conf");

    std::vector<int> ACTUAL = config.getInterventionChangeTimes();
    for (int i = 0; i < ACTUAL.size(); i++) {
        EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    }
}

TEST(ConfigurationTest, getOverdoseChangeTimes) {
    std::vector<int> EXPECTED = {51, 103, 155};

    Configuration config("TestInput/input1/sim.conf");

    std::vector<int> ACTUAL = config.getOverdoseChangeTimes();
    for (int i = 0; i < ACTUAL.size(); i++) {
        EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    }
}

TEST(ConfigurationTest, getDemographicCombos) {
    std::vector<std::string> EXPECTED = {" 10_14 male", " 10_14 female",
                                         " 15_19 male", " 15_19 female",
                                         " 20_24 male", " 20_24 female"};

    Configuration config("TestInput/input1/sim.conf");

    std::vector<std::string> ACTUAL = config.getDemographicCombos();
    for (int i = 0; i < ACTUAL.size(); i++) {
        EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    }
}

TEST(ConfigurationTest, getAgingInterval) {
    int EXPECTED = 260;

    Configuration config("TestInput/input1/sim.conf");

    int ACTUAL = config.getAgingInterval();
    EXPECT_EQ(EXPECTED, ACTUAL);
}

TEST(ConfigurationTest, getDuration) {
    int EXPECTED = 156;

    Configuration config("TestInput/input1/sim.conf");

    int ACTUAL = config.getDuration();
    EXPECT_EQ(EXPECTED, ACTUAL);
}

TEST(ConfigurationTest, getBool) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<bool> EXPECTED_VALUES = {true, true, false, false};
    std::vector<bool> REAL_VALUES = {
        TestConf.get<bool>("cost.cost_analysis"),
        TestConf.get<bool>("output.per_intervention_predictions"),
        TestConf.get<bool>("output.general_outputs"),
        TestConf.get<bool>("cost.cost_category_outputs")};

    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
}

TEST(ConfigurationTest, getInt) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<int> EXPECTED_VALUES = {156, 260, 2};
    std::vector<int> REAL_VALUES = {
        TestConf.get<int>("simulation.duration"),
        TestConf.get<int>("simulation.aging_interval"),
        TestConf.get<int>("cost.reporting_interval")};

    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
    // there is only one double to check
    EXPECT_EQ(TestConf.get<double>("cost.discount_rate"), 0.0025);
}

TEST(ConfigurationTest, getVectorInt) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<std::vector<int>> EXPECTED_VALUES = {
        {51, 103, 155}, {51, 103, 155}, {51, 103, 155}, {51, 103, 155}};
    std::vector<std::vector<int>> REAL_VALUES = {
        TestConf.get<std::vector<int>>("simulation.intervention_change_times"),
        TestConf.get<std::vector<int>>(
            "simulation.entering_sample_change_times"),
        TestConf.get<std::vector<int>>("simulation.overdose_change_times"),
        TestConf.get<std::vector<int>>(
            "output.general_stats_output_timesteps")};
    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
}

TEST(ConfigurationTest, getVectorString) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<std::vector<std::string>> EXPECTED_VALUES = {
        {"No_Treatment", "Buprenorphine", "Naltrexone", "Post-Buprenorphine",
         "Post-Naltrexone"},
        {"10_14", "15_19", "20_24"},
        {"male", "female"},
        {"Active_Noninjection", "Active_Injection", "Nonactive_Noninjection",
         "Nonactive_Injection"}};
    std::vector<std::vector<std::string>> REAL_VALUES = {
        TestConf.get<std::vector<std::string>>("state.interventions"),
        TestConf.get<std::vector<std::string>>("demographic.age_groups"),
        TestConf.get<std::vector<std::string>>("demographic.sex"),
        TestConf.get<std::vector<std::string>>("state.ouds")};
    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
}
