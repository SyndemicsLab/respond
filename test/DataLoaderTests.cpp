#include <filesystem>
#include <gtest/gtest.h>
#include "DataLoader.hpp"
#include "mocks/MockData.hpp"

class DataLoaderTest : public ::testing::Test {
protected:
    void SetUp () override {
        std::filesystem::path DEST_DIR("TestInput/");
        if (!std::filesystem::is_directory(DEST_DIR)) {
            std::filesystem::create_directories(DEST_DIR);
        }

        std::ofstream mockCSV("TestInput/simple.csv");
        mockCSV << "header1,header2,header3,header4" << std::endl
                << "1,2,3,4" << std::endl
                << "5,6,7,8" << std::endl
                << "9,10,11,12" << std::endl
                << "13,14,15,16" << std::endl;
        mockCSV.close();
        writeRealisticCSV();
    }

    void TearDown () override {
        std::filesystem::remove_all("TestInput");
    }
};

TEST_F(DataLoaderTest, SimpleHeaderCount) {
    DataLoader dataTest;
    InputTable realTable = dataTest.readCSV("TestInput/simple.csv");
    int EXPECTED = 4;
    EXPECT_EQ(realTable.size(), EXPECTED);
}

TEST_F(DataLoaderTest, SimpleHeaderContent) {
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

TEST_F(DataLoaderTest, SimpleNoMissingValues) {
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

TEST_F(DataLoaderTest, SimpleValueContent) {
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
