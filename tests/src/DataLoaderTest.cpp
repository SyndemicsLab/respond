//===-- DataLoaderTest.cpp - Instruction class definition -------*- C++ -*-===//
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
#include <iostream>

#include "DataLoader.hpp"

class DataLoaderTest : public ::testing::Test {
protected:
    boost::filesystem::path tempRelativeFile;
    boost::filesystem::path tempAbsoluteFile;
    boost::filesystem::path configFile;
    std::ofstream fileStream;
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
                            "cost_perspectives = Healthcare System, Societal, Policymakers" 
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

TEST_F(DataLoaderTest, Constructor) {
    Data::DataLoader dl;
    EXPECT_EQ(dl.getInterventions().size(), 0);
}

TEST_F(DataLoaderTest, ConstructorInputDirectory) {
    Data::DataLoader dl(boost::filesystem::temp_directory_path().string());
    std::cout << configFile << std::endl;
    std::cout << boost::filesystem::temp_directory_path().string() << std::endl;
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(DataLoaderTest, ConstructorIDandConfig) {}

TEST_F(DataLoaderTest, loadConfigurationFile) {}

TEST_F(DataLoaderTest, getDirName) {}

TEST_F(DataLoaderTest, getDuration) {}

TEST_F(DataLoaderTest, getNumOUDStates) {}

TEST_F(DataLoaderTest, getNumInterventions) {}

TEST_F(DataLoaderTest, getNumDemographics) {}

TEST_F(DataLoaderTest, getNumDemographicCombos) {}

TEST_F(DataLoaderTest, getInitialSample) {}

TEST_F(DataLoaderTest, getEnteringSamples) {}

TEST_F(DataLoaderTest, getOUDTransitionRates) {}

TEST_F(DataLoaderTest, getInterventionTransitionRates) {}

TEST_F(DataLoaderTest, getOverdoseRates) {}

TEST_F(DataLoaderTest, getFatalOverdoseRates) {}

TEST_F(DataLoaderTest, getMortalityRates) {}

TEST_F(DataLoaderTest, getInterventionInitRates) {}

TEST_F(DataLoaderTest, getInterventions) {}

TEST_F(DataLoaderTest, getOUDStates) {}

TEST_F(DataLoaderTest, getAgingInterval) {}

TEST_F(DataLoaderTest, getAgeGroupShift) {}

TEST_F(DataLoaderTest, getCostSwitch) {}

TEST_F(DataLoaderTest, getCostPerspectives) {}

TEST_F(DataLoaderTest, getDiscountRate) {}

TEST_F(DataLoaderTest, getReportingInterval) {}

TEST_F(DataLoaderTest, getCostCategoryOutputs) {}

TEST_F(DataLoaderTest, getCogetPerInterventionPredictionsstSwitch) {}

TEST_F(DataLoaderTest, getGeneralOutputsSwitch) {}

TEST_F(DataLoaderTest, getGeneralStatsOutputTimesteps) {}

TEST_F(DataLoaderTest, loadInitialSample) {}

TEST_F(DataLoaderTest, loadEnteringSamples) {}

TEST_F(DataLoaderTest, loadOUDTransitionRates) {}

TEST_F(DataLoaderTest, loadInterventionInitRates) {}

TEST_F(DataLoaderTest, loadInterventionTransitionRates) {}

TEST_F(DataLoaderTest, loadOverdoseRates) {}

TEST_F(DataLoaderTest, loadFatalOverdoseRates) {}

TEST_F(DataLoaderTest, loadMortalityRates) {}
