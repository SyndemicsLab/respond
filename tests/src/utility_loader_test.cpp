////////////////////////////////////////////////////////////////////////////////
// File: utility_loader_test.cpp                                              //
// Project: src                                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/utility_loader.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <respond/data_ops/matrices.hpp>

using namespace respond::data_ops;

class UtilityLoaderTest : public ::testing::Test {
protected:
    const std::string file_name = "util_test.csv";

    void SetUp() override {
        std::ofstream config_file_stream("sim.conf");

        config_file_stream
            << "[simulation]" << std::endl
            << "duration = 52" << std::endl
            << "aging_interval = 260" << std::endl
            << "intervention_change_times = 52" << std::endl
            << "entering_sample_change_times = 52" << std::endl
            << "overdose_change_times = 52" << std::endl
            << "stratified_entering_cohort = false" << std::endl
            << std::endl
            << "[state]" << std::endl
            << "interventions = No_Treatment, Buprenorphine,"
               "Naltrexone, Methadone, Detox, Post-Buprenorphine,"
               "Post-Naltrexone, Post-Methadone, Post-Detox"
            << std::endl
            << "ouds = Active_Noninjection, Active_Injection,"
               "Nonactive_Noninjection, Nonactive_Injection"
            << std::endl
            << std::endl
            << "[demographic]" << std::endl
            << "age_groups = 10_14, 15_19, 20_24, 25_29, 30_34, "
               "35_39, 40_44, 45_49, 50_54, 55_59, 60_64, 65_69, "
               "70_74, 75_79, 80_84, 85_89, 90_94, 95_99 "
            << std::endl
            << "sex = Male, Female " << std::endl
            << std::endl
            << "[cost]" << std::endl
            << "cost_analysis = true" << std::endl
            << "cost_perspectives = healthcare" << std::endl
            << "discount_rate = 0.0025 " << std::endl
            << "reporting_interval = 1" << std::endl
            << "cost_utility_output_timesteps = 52 " << std::endl
            << "cost_category_outputs = false " << std::endl
            << std::endl
            << "[output] " << std::endl
            << "per_intervention_predictions = true " << std::endl
            << "general_outputs = false " << std::endl
            << "general_stats_output_timesteps = 52";
        config_file_stream.close();
    }
    void TearDown() override {
        std::remove(file_name.c_str());
        std::remove("sim.conf");
    }
};

TEST_F(UtilityLoaderTest, LoadBackgroundUtility) {
    std::unique_ptr<UtilityLoader> utility_loader = UtilityLoader::Create();
    std::ofstream file_stream(file_name);
    file_stream << "agegrp,sex,utility" << std::endl
                << "10_14,Male,0.922" << std::endl
                << "10_14,Female,0.922";

    std::cout << "Testing file write" << std::endl;
    utility_loader->LoadBackgroundUtility(file_name);
    Matrix3d result = utility_loader->GetBackgroundUtility("utility");
    // EXPECT_EQ(result(0, 0, 0), 0.922);
}

TEST_F(UtilityLoaderTest, OUDUtility) {
    std::unique_ptr<UtilityLoader> utility_loader = UtilityLoader::Create();
    std::ofstream file_stream(file_name);
    file_stream << "block,oud,utility" << std::endl
                << "No_Treatment,Active_Noninjection,0.626" << std::endl
                << "No_Treatment,Active_Injection,0.512" << std::endl
                << "No_Treatment,Nonactive_Noninjection,1";

    file_stream.close();

    utility_loader->LoadOUDUtility(file_name);

    Matrix3d result = utility_loader->GetOUDUtility("utility");

    // EXPECT_EQ(result(0, 0, 0), 0.626);
}

TEST_F(UtilityLoaderTest, settingUtility) {
    std::unique_ptr<UtilityLoader> utility_loader = UtilityLoader::Create();
    std::ofstream file_stream(file_name);
    file_stream << "block,utility" << std::endl
                << "No_Treatment,1" << std::endl
                << "Buprenorphine,1" << std::endl
                << "Naltrexone,1";

    file_stream.close();

    utility_loader->LoadSettingUtility(file_name);

    Matrix3d result = utility_loader->GetSettingUtility("utility");

    // EXPECT_EQ(result(0, 0, 0), 1);
}
