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
#include <filesystem>
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
    std::filesystem::path tempRelativeFile;
    std::filesystem::path tempAbsoluteFile;
    std::ofstream outputFileStream;
    std::filesystem::path configFile;
    std::ofstream configFileStream;
    void SetUp() override {
        tempRelativeFile = std::tmpnam(nullptr) + std::string(".csv");
        tempAbsoluteFile =
            std::filesystem::temp_directory_path() / tempRelativeFile;

        configFile = std::filesystem::temp_directory_path() /
                     std::filesystem::path("sim.conf");
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
    Matrixify::History history;

    Matrixify::Matrix3d temp =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);

    Matrixify::Matrix4d stateHistory({temp});

    history.stateHistory = stateHistory;
    Calculator::PostSimulationCalculator calculator(history);
    EXPECT_EQ(calculator.getHistory().stateHistory(0, 0, 0, 0),
              history.stateHistory(0, 0, 0, 0));
}

TEST_F(PostSimulationCalculatorTest, calculateCost) {
    Matrixify::History history;
    Matrixify::Matrix3d temp =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Matrixify::Matrix4d stateHistory({temp});
    history.stateHistory = stateHistory;
    history.overdoseHistory = stateHistory;
    history.fatalOverdoseHistory = stateHistory;
    Calculator::PostSimulationCalculator calculator(history);

    std::shared_ptr<Matrixify::ICostLoader> costLoader =
        std::make_shared<MockCostLoader>();

    std::shared_ptr<MockCostLoader> mockedCostLoader =
        std::dynamic_pointer_cast<MockCostLoader>(costLoader);

    std::vector<std::string> retPerspectiveValue{"healthcare"};
    EXPECT_CALL(*mockedCostLoader, getCostPerspectives())
        .WillOnce(Return(retPerspectiveValue));

    EXPECT_CALL(*mockedCostLoader, getDiscountRate())
        .WillRepeatedly(Return(0.03));

    Matrixify::Matrix3d retCost =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(3.0);
    EXPECT_CALL(*mockedCostLoader, getHealthcareUtilizationCost("healthcare"))
        .WillRepeatedly(Return(retCost));

    EXPECT_CALL(*mockedCostLoader, getPharmaceuticalCost("healthcare"))
        .WillRepeatedly(Return(retCost));

    EXPECT_CALL(*mockedCostLoader, getTreatmentUtilizationCost("healthcare"))
        .WillRepeatedly(Return(retCost));

    double retNonFODCost = 100.0;
    EXPECT_CALL(*mockedCostLoader, getNonFatalOverdoseCost("healthcare"))
        .WillRepeatedly(Return(retNonFODCost));

    double retFODCost = 200.0;
    EXPECT_CALL(*mockedCostLoader, getFatalOverdoseCost("healthcare"))
        .WillRepeatedly(Return(retFODCost));

    Matrixify::CostList result = calculator.calculateCosts(costLoader);

    EXPECT_EQ(result[0].healthcareCost(0, 0, 0, 0), 6);
    EXPECT_EQ(result[0].pharmaCost(0, 0, 0, 0), 6);
    EXPECT_EQ(result[0].treatmentCost(0, 0, 0, 0), 6);
    EXPECT_EQ(result[0].nonFatalOverdoseCost(0, 0, 0, 0), 200);
    EXPECT_EQ(result[0].fatalOverdoseCost(0, 0, 0, 0), 400);
}

TEST_F(PostSimulationCalculatorTest, calculateUtility) {
    Matrixify::History history;
    Matrixify::Matrix3d temp =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Matrixify::Matrix4d stateHistory({temp});
    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    std::shared_ptr<Matrixify::IUtilityLoader> utilityLoader =
        std::make_shared<MockUtilityLoader>();

    std::shared_ptr<MockUtilityLoader> mockedUtilityLoader =
        std::dynamic_pointer_cast<MockUtilityLoader>(utilityLoader);

    Matrixify::Matrix3d retUtility =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(3.0);
    EXPECT_CALL(*mockedUtilityLoader, getBackgroundUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    EXPECT_CALL(*mockedUtilityLoader, getOUDUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    EXPECT_CALL(*mockedUtilityLoader, getSettingUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    Matrixify::Matrix4d result = calculator.calculateUtilities(
        utilityLoader, Calculator::UTILITY_TYPE::MIN);

    EXPECT_EQ(result(0, 0, 0, 0), 6);
}

TEST_F(PostSimulationCalculatorTest, calculateUtilityMin) {
    Matrixify::History history;
    Matrixify::Matrix3d temp =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Matrixify::Matrix4d stateHistory({temp});
    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    std::shared_ptr<Matrixify::IUtilityLoader> utilityLoader =
        std::make_shared<MockUtilityLoader>();

    std::shared_ptr<MockUtilityLoader> mockedUtilityLoader =
        std::dynamic_pointer_cast<MockUtilityLoader>(utilityLoader);

    Matrixify::Matrix3d retUtility =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(1.0);
    EXPECT_CALL(*mockedUtilityLoader, getBackgroundUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(0.75);
    EXPECT_CALL(*mockedUtilityLoader, getOUDUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(0.5);
    EXPECT_CALL(*mockedUtilityLoader, getSettingUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    Matrixify::Matrix4d result = calculator.calculateUtilities(
        utilityLoader, Calculator::UTILITY_TYPE::MIN);

    EXPECT_EQ(result(0, 0, 0, 0), 1);
}

TEST_F(PostSimulationCalculatorTest, calculateUtilityMult) {
    Matrixify::History history;
    Matrixify::Matrix3d temp =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(2.0);
    Matrixify::Matrix4d stateHistory({temp});
    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    std::shared_ptr<Matrixify::IUtilityLoader> utilityLoader =
        std::make_shared<MockUtilityLoader>();

    std::shared_ptr<MockUtilityLoader> mockedUtilityLoader =
        std::dynamic_pointer_cast<MockUtilityLoader>(utilityLoader);

    Matrixify::Matrix3d retUtility =
        Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(1.0);
    EXPECT_CALL(*mockedUtilityLoader, getBackgroundUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(0.75);
    EXPECT_CALL(*mockedUtilityLoader, getOUDUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    retUtility = Matrixify::Matrix3dFactory::Create(1, 1, 1).setConstant(0.5);
    EXPECT_CALL(*mockedUtilityLoader, getSettingUtility("utility"))
        .WillRepeatedly(Return(retUtility));

    Matrixify::Matrix4d result = calculator.calculateUtilities(
        utilityLoader, Calculator::UTILITY_TYPE::MULT);

    EXPECT_EQ(result(0, 0, 0, 0), 0.75);
}

TEST_F(PostSimulationCalculatorTest, calculateLifeYears) {
    Matrixify::History history;
    Matrixify::Matrix3d temp;
    Matrixify::Matrix4d stateHistory;
    for (int i = 0; i < 52; ++i) {
        temp =
            Matrixify::Matrix3dFactory::Create(2, 2, 2).setConstant(double(i));
        stateHistory.insert(temp, i);
    }

    history.stateHistory = stateHistory;

    Calculator::PostSimulationCalculator calculator(history);

    double result = calculator.calculateLifeYears();
    EXPECT_EQ(result, 204.0);
}
