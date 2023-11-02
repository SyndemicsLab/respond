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

#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

#include "Configuration.hpp"
#include "DataTypes.hpp"

class ConfigurationTest : public ::testing::Test {
protected:
    boost::filesystem::path configFile;
    std::ofstream configFileStream;
    void SetUp() override {
        tempRelativeFile =
            boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.csv");
        tempAbsoluteFile =
            boost::filesystem::temp_directory_path() / tempRelativeFile;
        configFile = boost::filesystem::temp_directory_path() /
                     boost::filesystem::path("sim.conf");
        configFileStream.open(configFile);

        // clang-format off
        configFileStream << "[simulation]" 
                            << std::endl << 
                            "duration = 52" 
                            << std::endl << 
                            "aging_interval = 260" 
                            << std::endl << 
                            "intervention_change_times = 52" 
                            << std::endl << 
                            "entering_sample_change_times = 52" 
                            << std::endl << 
                            "overdose_change_times = 52" 
                            << std::endl << std::endl << 
                            "[state]" 
                            << std::endl << 
                            "interventions = No_Treatment, Buprenorphine,"
                            "Naltrexone, Methadone, Detox, Post-Buprenorphine,"
                            "Post-Naltrexone, Post-Methadone, Post-Detox" ""
                            << std::endl << 
                            "ouds = Active_Noninjection, Active_Injection," 
                            "Nonactive_Noninjection, Nonactive_Injection" 
                            << std::endl << std::endl << 
                            "[demographic]" 
                            << std::endl << 
                            "age_groups = 10_14, 15_19, 20_24, 25_29, 30_34, "
                            "35_39, 40_44, 45_49, 50_54, 55_59, 60_64, 65_69, "
                            "70_74, 75_79, 80_84, 85_89, 90_94, 95_99 " 
                            << std::endl << 
                            "sex = Male, Female " 
                            << std::endl << std::endl <<
                            "[cost]" 
                            << std::endl << 
                            "cost_analysis = true" 
                            << std::endl <<  
                            "cost_perspectives = healthcare" 
                            << std::endl << 
                            "discount_rate = 0.0025 " 
                            << std::endl << 
                            "reporting_interval = 52 " 
                            << std::endl << 
                            "cost_category_outputs = false " 
                            << std::endl << std::endl << 
                            "[output] " 
                            << std::endl <<
                            "per_intervention_predictions = true " 
                            << std::endl <<
                            "general_outputs = false " 
                            << std::endl << 
                            "general_stats_output_timesteps = 52";
        // clang-format on
        configFileStream.close();
    }
    void TearDown() override {
        if (configFileStream.is_open()) {
            configFileStream.close();
        }
    }
};

TEST_F(ConfigurationTest, EmptyConstructor) {
    Configuration config;
    EXPECT_TRUE(&config != nullptr);
}

TEST_F(ConfigurationTest, GeneralConstructor) {
    // Configuration config("TestInput/input1/sim.conf");
    // EXPECT_TRUE(&config != nullptr);
}

TEST_F(ConfigurationTest, getInterventions) {
    // std::vector<std::string> EXPECTED = {"No_Treatment", "Buprenorphine",
    //                                      "Naltrexone", "Post-Buprenorphine",
    //                                      "Post-Naltrexone"};

    // Configuration config("TestInput/input1/sim.conf");

    // std::vector<std::string> ACTUAL = config.getInterventions();
    // for (int i = 0; i < ACTUAL.size(); i++) {
    //     EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    // }
}

TEST_F(ConfigurationTest, getOUDStates) {
    // std::vector<std::string> EXPECTED = {
    //     "Active_Noninjection", "Active_Injection", "Nonactive_Noninjection",
    //     "Nonactive_Injection"};

    // Configuration config("TestInput/input1/sim.conf");

    // std::vector<std::string> ACTUAL = config.getOUDStates();
    // for (int i = 0; i < ACTUAL.size(); i++) {
    //     EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    // }
}

TEST_F(ConfigurationTest, getNumDemographicCombos) {
    // int EXPECTED = 6;

    // Configuration config("TestInput/input1/sim.conf");

    // int ACTUAL = config.getNumDemographicCombos();
    // EXPECT_EQ(EXPECTED, ACTUAL);
}

TEST_F(ConfigurationTest, getEnteringSampleChangeTimes) {
    // std::vector<int> EXPECTED = {51, 103, 155};

    // Configuration config("TestInput/input1/sim.conf");

    // std::vector<int> ACTUAL = config.getEnteringSampleChangeTimes();
    // for (int i = 0; i < ACTUAL.size(); i++) {
    //     EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    // }
}

TEST_F(ConfigurationTest, getInterventionChangeTimes) {
    // std::vector<int> EXPECTED = {51, 103, 155};

    // Configuration config("TestInput/input1/sim.conf");

    // std::vector<int> ACTUAL = config.getInterventionChangeTimes();
    // for (int i = 0; i < ACTUAL.size(); i++) {
    //     EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    // }
}

TEST_F(ConfigurationTest, getOverdoseChangeTimes) {
    // std::vector<int> EXPECTED = {51, 103, 155};

    // Configuration config("TestInput/input1/sim.conf");

    // std::vector<int> ACTUAL = config.getOverdoseChangeTimes();
    // for (int i = 0; i < ACTUAL.size(); i++) {
    //     EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    // }
}

TEST_F(ConfigurationTest, getDemographicCombos) {
    // std::vector<std::string> EXPECTED = {" 10_14 male", " 10_14 female",
    //                                      " 15_19 male", " 15_19 female",
    //                                      " 20_24 male", " 20_24 female"};

    // Configuration config("TestInput/input1/sim.conf");

    // std::vector<std::string> ACTUAL = config.getDemographicCombos();
    // for (int i = 0; i < ACTUAL.size(); i++) {
    //     EXPECT_EQ(EXPECTED[i], ACTUAL[i]);
    // }
}

TEST_F(ConfigurationTest, getAgingInterval) {
    // int EXPECTED = 260;

    // Configuration config("TestInput/input1/sim.conf");

    // int ACTUAL = config.getAgingInterval();
    // EXPECT_EQ(EXPECTED, ACTUAL);
}

TEST_F(ConfigurationTest, getDuration) {
    // int EXPECTED = 156;

    // Configuration config("TestInput/input1/sim.conf");

    // int ACTUAL = config.getDuration();
    // EXPECT_EQ(EXPECTED, ACTUAL);
}

TEST_F(ConfigurationTest, getBool) {
    // Configuration TestConf("TestInput/input1/sim.conf");
    // std::vector<bool> EXPECTED_VALUES = {true, true, false, false};
    // std::vector<bool> REAL_VALUES = {
    //     TestConf.get<bool>("cost.cost_analysis"),
    //     TestConf.get<bool>("output.per_intervention_predictions"),
    //     TestConf.get<bool>("output.general_outputs"),
    //     TestConf.get<bool>("cost.cost_category_outputs")};

    // for (int i = 0; i < REAL_VALUES.size(); ++i) {
    //     EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    // }
}

TEST_F(ConfigurationTest, getInt) {
    // Configuration TestConf("TestInput/input1/sim.conf");
    // std::vector<int> EXPECTED_VALUES = {156, 260, 52};
    // std::vector<int> REAL_VALUES = {
    //     TestConf.get<int>("simulation.duration"),
    //     TestConf.get<int>("simulation.aging_interval"),
    //     TestConf.get<int>("cost.reporting_interval")};

    // for (int i = 0; i < REAL_VALUES.size(); ++i) {
    //     EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    // }
    // // there is only one double to check
    // EXPECT_EQ(TestConf.get<double>("cost.discount_rate"), 0.0025);
}

TEST_F(ConfigurationTest, getVectorInt) {
    // Configuration TestConf("TestInput/input1/sim.conf");
    // std::vector<std::vector<int>> EXPECTED_VALUES = {
    //     {51, 103, 155}, {51, 103, 155}, {51, 103, 155}, {51, 103, 155}};
    // std::vector<std::vector<int>> REAL_VALUES = {
    //     TestConf.get<std::vector<int>>("simulation.intervention_change_times"),
    //     TestConf.get<std::vector<int>>(
    //         "simulation.entering_sample_change_times"),
    //     TestConf.get<std::vector<int>>("simulation.overdose_change_times"),
    //     TestConf.get<std::vector<int>>(
    //         "output.general_stats_output_timesteps")};
    // for (int i = 0; i < REAL_VALUES.size(); ++i) {
    //     EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    // }
}

TEST_F(ConfigurationTest, getVectorString) {
    // Configuration TestConf("TestInput/input1/sim.conf");
    // std::vector<std::vector<std::string>> EXPECTED_VALUES = {
    //     {"No_Treatment", "Buprenorphine", "Naltrexone", "Post-Buprenorphine",
    //      "Post-Naltrexone"},
    //     {"10_14", "15_19", "20_24"},
    //     {"male", "female"},
    //     {"Active_Noninjection", "Active_Injection", "Nonactive_Noninjection",
    //      "Nonactive_Injection"}};
    // std::vector<std::vector<std::string>> REAL_VALUES = {
    //     TestConf.get<std::vector<std::string>>("state.interventions"),
    //     TestConf.get<std::vector<std::string>>("demographic.age_groups"),
    //     TestConf.get<std::vector<std::string>>("demographic.sex"),
    //     TestConf.get<std::vector<std::string>>("state.ouds")};
    // for (int i = 0; i < REAL_VALUES.size(); ++i) {
    //     EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    // }
}
