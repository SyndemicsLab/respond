//===-----------------------------*- C++ -*--------------------------------===//
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

#include "gmock/gmock.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include "MockCostLoader.hpp"
#include "MockDataLoader.hpp"
#include "PostSimulationCalculator.hpp"

class PostSimulationCalculatorTest : public ::testing::Test {
protected:
    boost::filesystem::path tempRelativeFile;
    boost::filesystem::path tempAbsoluteFile;
    std::ofstream outputFileStream;
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
                            "discount_rate = 0.03" 
                            << std::endl << 
                            "reporting_interval = 52" 
                            << std::endl << 
                            "cost_category_outputs = false " 
                            << std::endl << std::endl << 
                            "[output] " 
                            << std::endl <<
                            "per_intervention_predictions = true " 
                            << std::endl <<
                            "general_outputs = false" 
                            << std::endl << 
                            "general_stats_output_timesteps = 52";
        // clang-format on
        configFileStream.close();
    }
    void TearDown() override {
        if (outputFileStream.is_open()) {
            outputFileStream.close();
        }
        if (configFileStream.is_open()) {
            configFileStream.close();
        }
    }
};

TEST_F(PostSimulationCalculatorTest, constructor) {
    Data::History history;

    Data::Matrix3d temp =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);

    Data::Matrix3dOverTime stateHistory({temp});

    history.stateHistory = stateHistory;
    Calculator::PostSimulationCalculator calculator(history);
    EXPECT_EQ(calculator.getHistory().stateHistory(0, 0, 0, 0),
              history.stateHistory(0, 0, 0, 0));
}

TEST_F(PostSimulationCalculatorTest, calculateCost) {
    Data::History history;
    Data::Matrix3d temp =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Data::Matrix3dOverTime stateHistory({temp});
    history.stateHistory = stateHistory;
    Calculator::PostSimulationCalculator calculator(history);
    MockCostLoader costLoader;
    EXPECT_CALL(costLoader, getHealthcareUtilizationCost("healthcare"));

    calculator.calculateCost(costLoader);
}

TEST_F(PostSimulationCalculatorTest, calculateUtility) {}
