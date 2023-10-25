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

#include "CostLoader.hpp"

class CostLoaderTest : public ::testing::Test {
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

TEST_F(CostLoaderTest, Constructor) {
    Data::CostLoader cl;
    Data::Matrix3d result = cl.getHealthcareUtilizationCost("healthcare");
    EXPECT_EQ(result.size(), 0);
}

TEST_F(CostLoaderTest, ConstructorStr) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    EXPECT_EQ(cl.getConfiguration().getInterventions().size(), 9);
}

TEST_F(CostLoaderTest, loadConfigurationFile) {
    Data::CostLoader cl;
    cl.loadConfigurationFile(configFile.string());
    EXPECT_EQ(cl.getConfiguration().getInterventions().size(), 9);
}

TEST_F(CostLoaderTest, healthcareUtilizationCost) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    fileStream << "block,agegrp,sex,oud,healthcare" << std::endl
               << "No_Treatment,10_14,Male,Active_Noninjection,243" << std::endl
               << "No_Treatment,10_14,Male,Active_Injection,355.96" << std::endl
               << "No_Treatment,10_14,Male,Nonactive_Noninjection,162.22";

    fileStream.close();

    cl.loadHealthcareUtilizationCost(tempAbsoluteFile.string());
    Data::Matrix3d result = cl.getHealthcareUtilizationCost("healthcare");
    EXPECT_EQ(result(0, 0, 0), 243);
}

TEST_F(CostLoaderTest, overdoseCost) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    fileStream << "X,healthcare" << std::endl
               << "non_fatal_overdose,4557.35" << std::endl
               << "fatal_overdose,857.97";

    fileStream.close();

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
        output = cl.loadOverdoseCost(tempAbsoluteFile.string());

    EXPECT_EQ(output["healthcare"]["non_fatal_overdose"], 4557.35);
    EXPECT_EQ(cl.getNonFatalOverdoseCost("healthcare"), 4557.35);
}

TEST_F(CostLoaderTest, pharmaceuticalCost) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    fileStream << "block,healthcare" << std::endl
               << "Buprenorphine,48.71" << std::endl
               << "Naltrexone,302.58" << std::endl
               << "Methadone,4.31" << std::endl
               << "Detox,0";

    fileStream.close();

    std::unordered_map<std::string, Data::Matrix3d> output =
        cl.loadPharmaceuticalCost(tempAbsoluteFile.string());

    Data::Matrix3d result = cl.getPharmaceuticalCost("healthcare");

    EXPECT_EQ(output["healthcare"](1, 0, 0), 48.71);
    EXPECT_EQ(result(1, 0, 0), 48.71);
}

TEST_F(CostLoaderTest, treatmentUtilizationCost) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    fileStream << "block,healthcare" << std::endl
               << "Buprenorphine,65.24" << std::endl
               << "Naltrexone,24.36" << std::endl
               << "Methadone,123.43" << std::endl
               << "Detox,2863";

    fileStream.close();

    std::unordered_map<std::string, Data::Matrix3d> output =
        cl.loadTreatmentUtilizationCost(tempAbsoluteFile.string());

    Data::Matrix3d result = cl.getTreatmentUtilizationCost("healthcare");

    EXPECT_EQ(output["healthcare"](1, 0, 0), 65.24);
    EXPECT_EQ(result(1, 0, 0), 65.24);
}

TEST_F(CostLoaderTest, getNonFatalOverdoseCost) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    fileStream << "X,healthcare" << std::endl
               << "non_fatal_overdose,4557.35" << std::endl
               << "fatal_overdose,857.97";

    fileStream.close();
    cl.loadOverdoseCost(tempAbsoluteFile.string());
    EXPECT_EQ(cl.getNonFatalOverdoseCost("healthcare"), 4557.35);
}

TEST_F(CostLoaderTest, getFatalOverdoseCost) {
    Data::CostLoader cl(boost::filesystem::temp_directory_path().string());
    fileStream << "X,healthcare" << std::endl
               << "non_fatal_overdose,4557.35" << std::endl
               << "fatal_overdose,857.97";

    fileStream.close();
    cl.loadOverdoseCost(tempAbsoluteFile.string());
    EXPECT_EQ(cl.getFatalOverdoseCost("healthcare"), 857.97);
}