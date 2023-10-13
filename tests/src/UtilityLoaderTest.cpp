//===----------------------------*- C++ -*---------------------------------===//
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

#include "UtilityLoader.hpp"

class UtilityLoaderTest : public ::testing::Test {
protected:
    boost::filesystem::path tempRelativeFile;
    boost::filesystem::path tempAbsoluteFile;
    boost::filesystem::path configFile;
    std::ofstream configFileStream;
    std::ofstream fileStream;
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
        fileStream.open(tempAbsoluteFile);
    }
    void TearDown() override {
        if (configFileStream.is_open()) {
            configFileStream.close();
        }
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
};

TEST_F(UtilityLoaderTest, Constructor) {
    Data::UtilityLoader ul;
    Data::Matrix3d result = ul.getBackgroundUtility();
    EXPECT_EQ(result.size(), 0);
}

TEST_F(UtilityLoaderTest, ConstructorStr) {
    Data::UtilityLoader ul(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(ul.getConfiguration().getInterventions().size(), 9);
}

TEST_F(UtilityLoaderTest, loadConfigurationFile) {
    Data::UtilityLoader ul;
    ul.loadConfigurationFile(configFile.string());
    EXPECT_EQ(ul.getConfiguration().getInterventions().size(), 9);
}

TEST_F(UtilityLoaderTest, backgroundUtility) {
    Data::UtilityLoader ul(boost::filesystem::temp_directory_path().string());
    fileStream << "agegrp,sex,utility" << std::endl
               << "10_14,Male,0.922" << std::endl
               << "10_14,Female,0.922" << std::endl
               << "15_19,Male,0.922";

    fileStream.close();

    Data::Matrix3d output = ul.loadBackgroundUtility(tempAbsoluteFile.string());

    Data::Matrix3d result = ul.getBackgroundUtility();

    EXPECT_EQ(output(0, 0, 0), 0.922);
    EXPECT_EQ(result(0, 0, 0), 0.922);
}

TEST_F(UtilityLoaderTest, OUDUtility) {
    Data::UtilityLoader ul(boost::filesystem::temp_directory_path().string());
    fileStream << "block,oud,utility" << std::endl
               << "No_Treatment,Active_Noninjection,0.626" << std::endl
               << "No_Treatment,Active_Injection,0.512" << std::endl
               << "No_Treatment,Nonactive_Noninjection,1";

    fileStream.close();

    Data::Matrix3d output = ul.loadOUDUtility(tempAbsoluteFile.string());

    Data::Matrix3d result = ul.getOUDUtility();

    EXPECT_EQ(output(0, 0, 0), 0.626);
    EXPECT_EQ(result(0, 0, 0), 0.626);
}

TEST_F(UtilityLoaderTest, settingUtility) {
    Data::UtilityLoader ul(boost::filesystem::temp_directory_path().string());
    fileStream << "block,utility" << std::endl
               << "No_Treatment,1" << std::endl
               << "Buprenorphine,1" << std::endl
               << "Naltrexone,1";

    fileStream.close();

    Data::Matrix3d output = ul.loadSettingUtility(tempAbsoluteFile.string());

    Data::Matrix3d result = ul.getSettingUtility();

    EXPECT_EQ(output(0, 0, 0), 1);
    EXPECT_EQ(result(0, 0, 0), 1);
}
