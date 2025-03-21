////////////////////////////////////////////////////////////////////////////////
// File: writer_test.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/writer.hpp>

#include <fstream>
#include <memory>

#include <datamanagement/DataManagement.hpp>
#include <gtest/gtest.h>

#include "data_loader_mock.hpp"

using namespace respond::data_ops;

class WriterTest : public ::testing::Test {
protected:
    std::unique_ptr<Writer> writer;
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
        config_file_stream.close();

        Data::IConfigablePtr config =
            std::make_shared<Data::Config>("sim.conf");
        writer = Writer::Create(config);
    }
    void TearDown() override { std::remove("sim.conf"); }
};

TEST_F(WriterTest, FactoryCreate) {
    auto w = Writer::Create(nullptr);
    EXPECT_NE(nullptr, w);
}

TEST_F(WriterTest, WriteInputData) {
    // MockDataLoader data_loader;
    // writer->WriteInputData(data_loader, "", OutputType::kString);
}