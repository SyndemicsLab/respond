#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>

#include "spdlog/spdlog.h"

#include "DataLoader.hpp"
// #include <DataManagement.hpp>
#include "DataManagement.hpp"

class BaseLoaderTest : public ::testing::Test {
protected:
    std::filesystem::path tempRelativeFile;
    std::filesystem::path tempAbsoluteFile;
    std::filesystem::path tempRelativeFile2;
    std::filesystem::path tempAbsoluteFile2;
    std::filesystem::path configFile;
    std::ofstream fileStream;
    std::ofstream fileStream2;
    std::ofstream configFileStream;
    std::shared_ptr<spdlog::logger> logger;
    void SetUp() override {
        if (!logger) {
            if (spdlog::get("test")) {
                logger = spdlog::get("test");
            } else {
                logger = spdlog::stdout_color_mt("test");
            }
        }
        tempRelativeFile = std::tmpnam(nullptr) + std::string(".csv");
        tempAbsoluteFile =
            std::filesystem::temp_directory_path() / tempRelativeFile;

        tempRelativeFile2 = std::tmpnam(nullptr) + std::string(".csv");
        tempAbsoluteFile2 =
            std::filesystem::temp_directory_path() / tempRelativeFile2;

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
                            << std::endl << 
                            "stratified_entering_cohort = false" 
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
                            "reporting_interval = 1"
                            << std::endl << 
                            "cost_utility_output_timesteps = 52 " 
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
        fileStream2.open(tempAbsoluteFile2);
    }
    void TearDown() override {
        if (configFileStream.is_open()) {
            configFileStream.close();
        }
        if (fileStream.is_open()) {
            fileStream.close();
        }
        if (fileStream2.is_open()) {
            fileStream2.close();
        }
    }
};

TEST_F(BaseLoaderTest, getDuration) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getDuration(), 52);
}

TEST_F(BaseLoaderTest, getNumOUDStates) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getNumOUDStates(), 4);
}

TEST_F(BaseLoaderTest, getNumInterventions) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getNumInterventions(), 9);
}

TEST_F(BaseLoaderTest, getNumDemographics) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getNumDemographics(), 2);
}

TEST_F(BaseLoaderTest, getNumDemographicCombos) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getNumDemographicCombos(), 36);
}

TEST_F(BaseLoaderTest, getInterventions) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getInterventions().size(), 9);
}

TEST_F(BaseLoaderTest, getOUDStates) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getOUDStates().size(), 4);
}

TEST_F(BaseLoaderTest, getAgingInterval) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getAgingInterval(), 260);
}

TEST_F(BaseLoaderTest, getAgeGroupShift) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getAgeGroupShift(), 5);
}

TEST_F(BaseLoaderTest, getCostSwitch) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_TRUE(dl.getCostSwitch());
}

TEST_F(BaseLoaderTest, getCostPerspectives) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getCostPerspectives()[0], "healthcare");
}

TEST_F(BaseLoaderTest, getDiscountRate) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getDiscountRate(), 0.0025);
}

TEST_F(BaseLoaderTest, getCostUtilityOutputTimesteps) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getCostUtilityOutputTimesteps()[0], 52);
}

TEST_F(BaseLoaderTest, getCostCategoryOutputs) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_FALSE(dl.getCostCategoryOutputs());
}

TEST_F(BaseLoaderTest, getPerInterventionPredictions) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_TRUE(dl.getPerInterventionPredictions());
}

TEST_F(BaseLoaderTest, getGeneralOutputsSwitch) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_FALSE(dl.getGeneralOutputsSwitch());
}

TEST_F(BaseLoaderTest, getGeneralStatsOutputTimesteps) {
    matrixify::DataLoader dl(std::filesystem::temp_directory_path().string(),
                             logger);
    EXPECT_EQ(dl.getGeneralStatsOutputTimesteps().size(), 1);
}