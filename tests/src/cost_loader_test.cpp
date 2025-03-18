////////////////////////////////////////////////////////////////////////////////
// File: TEST_CostLoader.cpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/cost_loader.hpp>

#include <cstdio>
#include <fstream>
#include <memory>

#include <gtest/gtest.h>

#include <respond/data_ops/matrices.hpp>

using namespace respond::data_ops;

class CostLoaderTest : public ::testing::Test {
protected:
    const std::string file_name = "test.csv";
    std::unique_ptr<CostLoader> cost_loader;
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
               ""
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

        cost_loader = CostLoader::Create();
    }
    void TearDown() override {
        std::remove(file_name.c_str());
        std::remove("sim.conf");
    }
};

TEST_F(CostLoaderTest, FactoryCreate) {
    auto cl = CostLoader::Create();
    EXPECT_NE(nullptr, cl);
}

TEST_F(CostLoaderTest, HealthcareUtilizationCost) {
    std::ofstream file_stream(file_name);
    file_stream << "block,agegrp,sex,oud,healthcare" << std::endl
                << "No_Treatment,10_14,Male,Active_Noninjection,243"
                << std::endl
                << "No_Treatment,10_14,Male,Active_Injection,355.96"
                << std::endl
                << "No_Treatment,10_14,Male,Nonactive_Noninjection,162.22";

    cost_loader->LoadHealthcareUtilizationCost(file_name);
    Matrix3d result = cost_loader->GetHealthcareUtilizationCost("healthcare");
    EXPECT_EQ(result(0, 0, 0), 243);
}

TEST_F(CostLoaderTest, OverdoseCost) {
    std::ofstream file_stream(file_name);
    file_stream << "X,healthcare" << std::endl
                << "non_fatal_overdose,4557.35" << std::endl
                << "fatal_overdose,857.97";

    file_stream.close();

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
        output = cost_loader->LoadOverdoseCost(file_name);

    EXPECT_EQ(output["healthcare"]["non_fatal_overdose"], 4557.35);
    EXPECT_EQ(cost_loader->GetNonFatalOverdoseCost("healthcare"), 4557.35);
}

TEST_F(CostLoaderTest, pharmaceuticalCost) {
    std::ofstream file_stream(file_name);
    file_stream << "block,healthcare" << std::endl
                << "Buprenorphine,48.71" << std::endl
                << "Naltrexone,302.58" << std::endl
                << "Methadone,4.31" << std::endl
                << "Detox,0";

    file_stream.close();

    auto output = cost_loader->LoadPharmaceuticalCost(file_name);

    EXPECT_EQ(output["healthcare"](1, 0, 0), 48.71);

    Matrix3d result = cost_loader->GetPharmaceuticalCost("healthcare");

    EXPECT_EQ(result(1, 0, 0), 48.71);
}

TEST_F(CostLoaderTest, treatmentUtilizationCost) {
    std::ofstream file_stream(file_name);
    file_stream << "block,healthcare" << std::endl
                << "Buprenorphine,65.24" << std::endl
                << "Naltrexone,24.36" << std::endl
                << "Methadone,123.43" << std::endl
                << "Detox,2863";

    file_stream.close();

    std::unordered_map<std::string, Matrix3d> output =
        cost_loader->LoadTreatmentUtilizationCost(file_name);

    Matrix3d result = cost_loader->GetTreatmentUtilizationCost("healthcare");

    EXPECT_EQ(output["healthcare"](1, 0, 0), 65.24);
    EXPECT_EQ(result(1, 0, 0), 65.24);
}

TEST_F(CostLoaderTest, getNonFatalOverdoseCost) {
    std::ofstream file_stream(file_name);
    file_stream << "X,healthcare" << std::endl
                << "non_fatal_overdose,4557.35" << std::endl
                << "fatal_overdose,857.97";

    file_stream.close();
    cost_loader->LoadOverdoseCost(file_name);
    EXPECT_EQ(cost_loader->GetNonFatalOverdoseCost("healthcare"), 4557.35);
}

TEST_F(CostLoaderTest, getFatalOverdoseCost) {
    std::ofstream file_stream(file_name);
    file_stream << "X,healthcare" << std::endl
                << "non_fatal_overdose,4557.35" << std::endl
                << "fatal_overdose,857.97";

    file_stream.close();
    cost_loader->LoadOverdoseCost(file_name);
    EXPECT_EQ(cost_loader->GetFatalOverdoseCost("healthcare"), 857.97);
}
