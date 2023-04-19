#include <gtest/gtest.h>
#include "configuration.hpp"
#include "DataLoader.hpp"

using namespace Data;

TEST(DataLoaderTest, DefaultConstructor) {
    DataLoader dataTest;
    std::string EXPECTED = "";
    EXPECT_EQ(dataTest.getDirName(), EXPECTED);
}

TEST(DataLoaderTest, CSVHeaderCount) {
    DataLoader dataTest;
    InputTable realTable = dataTest.readCSV("TestInput/simple.csv");
    int EXPECTED = 4;
    EXPECT_EQ(realTable.size(), EXPECTED);
}

TEST(DataLoaderTest, CSVHeaderContent) {
    DataLoader dataTest;
    InputTable realTable = dataTest.readCSV("TestInput/simple.csv");
    std::vector<std::string> EXPECTED_HEADERS = {
        "header1", "header2", "header3", "header4"
    };
    std::vector<std::string> REAL_HEADERS;
    for (int i = 0; i < EXPECTED_HEADERS.size(); ++i) {
        const auto& iter = realTable.find(EXPECTED_HEADERS[i]);
        REAL_HEADERS.push_back(iter->first);
    }
    EXPECT_EQ(REAL_HEADERS, EXPECTED_HEADERS);
}

TEST(DataLoaderTest, CSVNoMissingValues) {
    DataLoader dataTest;
    InputTable realTable = dataTest.readCSV("TestInput/simple.csv");
    std::vector<std::string> HEADERS = {
        "header1", "header2", "header3", "header4"
    };
    int EXPECTED_LENGTH = 4;
    std::vector<std::string> REAL_VALUES;
    for (int i = 0; i < HEADERS.size(); ++i) {
        const auto& iter = realTable.find(HEADERS[i]);
        REAL_VALUES = iter->second;
        EXPECT_EQ(REAL_VALUES.size(), EXPECTED_LENGTH);
    }
}

TEST(DataLoaderTest, CSVValueContent) {
    DataLoader dataTest;
    InputTable realTable = dataTest.readCSV("TestInput/simple.csv");
    std::vector<std::string> HEADERS = {
        "header1", "header2", "header3", "header4"
    };
    std::vector<std::vector<std::string>> EXPECTED_VALUES = {
        {"1", "5", "9", "13"},
        {"2", "6", "10", "14"},
        {"3", "7", "11", "15"},
        {"4", "8", "12", "16"}
    };
    std::vector<std::string> REAL_VALUES;
    for (int i = 0; i < HEADERS.size(); ++i) {
        const auto& iter = realTable.find(HEADERS[i]);
        REAL_VALUES = iter->second;
        EXPECT_EQ(REAL_VALUES, EXPECTED_VALUES[i]);
    }
}

TEST(DataLoaderTest, ConfigFileBool) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<bool> EXPECTED_VALUES = {
        true,
        true,
        false,
        false
    };
    std::vector<bool> REAL_VALUES = {
        TestConf.Get<bool>("cost.cost_analysis"),
        TestConf.Get<bool>("output.per_intervention_predictions"),
        TestConf.Get<bool>("output.general_outputs"),
        TestConf.Get<bool>("output.cost_category_outputs")
    };

    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
}

TEST(DataLoaderTest, ConfigFileInt) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<int> EXPECTED_VALUES = {
        156,
        260,
        2
    };
    std::vector<int> REAL_VALUES = {
        TestConf.Get<int>("simulation.duration"),
        TestConf.Get<int>("simulation.aging_interval"),
        TestConf.Get<int>("output.reporting_interval")
    };

    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
    // there is only one double to check
    EXPECT_EQ(TestConf.Get<double>("cost.discount_rate"), 0.0025);
}

TEST(DataLoaderTest, ConfigFileIntVector) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<std::vector<int>> EXPECTED_VALUES = {
        {52, 104, 156},
        {52, 104, 156},
        {52, 104, 156},
        {52, 104, 156}
    };
    std::vector<std::vector<int>> REAL_VALUES = {
        TestConf.Get<std::vector<int>>("simulation.intervention_change_times"),
        TestConf.Get<std::vector<int>>("simulation.entering_sample_change_times"),
        TestConf.Get<std::vector<int>>("simulation.overdose_change_times"),
        TestConf.Get<std::vector<int>>("output.general_stats_output_timesteps")
    };
    for (int i = 0; i < REAL_VALUES.size(); ++i) {
        EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    }
}

TEST(DataLoaderTest, ConfigFileStringVector) {
    Configuration TestConf("TestInput/input1/sim.conf");
    std::vector<std::vector<std::string>> EXPECTED_VALUES = {
        {"No_Treatment", "Buprenorphine", "Naltrexone", "Post-Buprenorphine", "Post-Naltrexone"},
        {"10_14", "15_19", "20_24"},
        {"male", "female"},
        {"Active_Noninjection", "Active_Injection", "Nonactive_Noninjection", "Nonactive_Injection"}
    };
    // std::vector<std::vector<std::string>> REAL_VALUES = {
    //     TestConf.Get<std::vector<std::string>>("state.interventions"),
    //     TestConf.Get<std::vector<std::string>>("demographic.age_groups"),
    //     TestConf.Get<std::vector<std::string>>("demographic.sex"),
    //     TestConf.Get<std::vector<std::string>>("state.oud_states")
    // };
    // for (int i = 0; i < REAL_VALUES.size(); ++i) {
    //     EXPECT_EQ(REAL_VALUES[i], EXPECTED_VALUES[i]);
    // }
}
