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
#include <iostream>
#include <string>
#include <vector>

#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include "Matrix3dPrinter.hpp"
#include "MockCostLoader.hpp"
#include "MockDataLoader.hpp"
#include "MockUtilityLoader.hpp"
#include "PostSimulationCalculator.hpp"

using ::testing::Return;

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
                            "cost_utility_output_timesteps = 52" 
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
    history.overdoseHistory = stateHistory;
    history.fatalOverdoseHistory = stateHistory;
    Calculator::PostSimulationCalculator calculator(history);

    MockCostLoader costLoader;

    std::vector<std::string> retPerspectiveValue{"healthcare"};
    EXPECT_CALL(costLoader, getCostPerspectives())
        .WillOnce(Return(retPerspectiveValue));

    EXPECT_CALL(costLoader, getDiscountRate()).WillRepeatedly(Return(0.03));

    Data::Matrix3d retCost =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(3.0);
    EXPECT_CALL(costLoader, getHealthcareUtilizationCost("healthcare"))
        .WillRepeatedly(Return(retCost));

    EXPECT_CALL(costLoader, getPharmaceuticalCost("healthcare"))
        .WillRepeatedly(Return(retCost));

    EXPECT_CALL(costLoader, getTreatmentUtilizationCost("healthcare"))
        .WillRepeatedly(Return(retCost));

    double retNonFODCost = 100.0;
    EXPECT_CALL(costLoader, getNonFatalOverdoseCost("healthcare"))
        .WillRepeatedly(Return(retNonFODCost));

    double retFODCost = 200.0;
    EXPECT_CALL(costLoader, getFatalOverdoseCost("healthcare"))
        .WillRepeatedly(Return(retFODCost));

    Data::CostList result = calculator.calculateCosts(costLoader);

    EXPECT_EQ(result[0].healthcareCost(0, 0, 0, 0), 5);
    EXPECT_EQ(result[0].pharmaCost(0, 0, 0, 0), 5);
    EXPECT_EQ(result[0].treatmentCost(0, 0, 0, 0), 5);
    EXPECT_EQ(result[0].nonFatalOverdoseCost(0, 0, 0, 0), 199);
    EXPECT_EQ(result[0].fatalOverdoseCost(0, 0, 0, 0), 399);
}

TEST_F(PostSimulationCalculatorTest, calculateUtility) {
    Data::History history;
    Data::Matrix3d temp =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Data::Matrix3dOverTime stateHistory({temp});
    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    MockUtilityLoader utilityLoader;

    Data::Matrix3d retUtility =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(3.0);
    EXPECT_CALL(utilityLoader, getBackgroundUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    EXPECT_CALL(utilityLoader, getOUDUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    EXPECT_CALL(utilityLoader, getSettingUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    Data::Matrix3dOverTime result = calculator.calculateUtilities(
        utilityLoader, Calculator::UTILITY_TYPE::MIN);

    EXPECT_EQ(result(0, 0, 0, 0), 6);
}

TEST_F(PostSimulationCalculatorTest, calculateUtilityMin) {
    Data::History history;
    Data::Matrix3d temp =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Data::Matrix3dOverTime stateHistory({temp});
    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    MockUtilityLoader utilityLoader;

    Data::Matrix3d retUtility =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(1.0);
    EXPECT_CALL(utilityLoader, getBackgroundUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(0.75);
    EXPECT_CALL(utilityLoader, getOUDUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(0.5);
    EXPECT_CALL(utilityLoader, getSettingUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    Data::Matrix3dOverTime result = calculator.calculateUtilities(
        utilityLoader, Calculator::UTILITY_TYPE::MIN);

    EXPECT_EQ(result(0, 0, 0, 0), 1);
}

TEST_F(PostSimulationCalculatorTest, calculateUtilityMult) {
    Data::History history;
    Data::Matrix3d temp =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Data::Matrix3dOverTime stateHistory({temp});
    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    MockUtilityLoader utilityLoader;

    Data::Matrix3d retUtility =
        Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(1.0);
    EXPECT_CALL(utilityLoader, getBackgroundUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(0.75);
    EXPECT_CALL(utilityLoader, getOUDUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Utilities::Matrix3dFactory::Create(1, 1, 1).setConstant(0.5);
    EXPECT_CALL(utilityLoader, getSettingUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    Data::Matrix3dOverTime result = calculator.calculateUtilities(
        utilityLoader, Calculator::UTILITY_TYPE::MULT);

    EXPECT_EQ(result(0, 0, 0, 0), 0.75);
}
