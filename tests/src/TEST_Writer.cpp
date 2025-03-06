////////////////////////////////////////////////////////////////////////////////
// File: TEST_Writer.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include <respondsimulation/data_ops/Writer.hpp>

class WriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(WriterTest, ConstructorStringAndVectorAndWriteType) {
    data_ops::Writer writer("test", {}, data_ops::WriteType::STRING);
    std::string result = writer.getDirname();
    EXPECT_EQ(result, std::string("test"));
}

TEST_F(WriterTest, writeType) {
    data_ops::Writer writer;
    writer.setWriteType(data_ops::WriteType::FILE);
    EXPECT_EQ(writer.getWriteType(), data_ops::WriteType::FILE);
}

TEST_F(WriterTest, dirname) {
    data_ops::Writer writer;
    writer.setDirname("testingDirname");
    EXPECT_EQ(writer.getDirname(), std::string("testingDirname"));
}

TEST_F(WriterTest, timesteps) {
    std::vector<int> timesteps{0, 2, 4, 6, 8};
    data_ops::Writer writer("", timesteps);
    EXPECT_EQ(writer.getTimesteps(), timesteps);
}

class OutputWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(OutputWriterTest, ConstructorEmpty) {
    data_ops::OutputWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(OutputWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    data_ops::OutputWriter writer("testDirname", {"inte"}, {"oud"}, {"dem"},
                                  {"demCom"}, {1}, data_ops::WriteType::STRING,
                                  false);
    EXPECT_EQ(writer.getDirname(), std::string("testDirname"));
}

class HistoryWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HistoryWriterTest, ConstructorEmpty) {
    data_ops::HistoryWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(HistoryWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    data_ops::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   data_ops::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testHistoryDirname"));
}

TEST_F(HistoryWriterTest, writeHistory) {
    data_ops::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   data_ops::WriteType::STRING, false);

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    data_ops::Matrix4d mat4d({mat3d});
    data_ops::History hist;
    hist.stateHistory = mat4d;
    hist.overdoseHistory = mat4d;
    hist.fatalOverdoseHistory = mat4d;
    hist.mortalityHistory = mat4d;
    hist.interventionAdmissionHistory = mat4d;
    std::string result = writer.writeHistory(hist);

    std::string expected =
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n";
    EXPECT_EQ(result, expected);
}

TEST_F(HistoryWriterTest, writeHistoryError) {
    data_ops::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   data_ops::WriteType::STRING, false);

    data_ops::Matrix3d mat3d(1, 1, 1);
    data_ops::History hist;
    std::string result = writer.writeHistory(hist);

    std::string expected = "failure";
    EXPECT_EQ(result, expected);
}

class CostWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CostWriterTest, ConstructorEmpty) {
    data_ops::CostWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(CostWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    data_ops::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                {"demCom"}, {1}, data_ops::WriteType::STRING,
                                false);
    EXPECT_EQ(writer.getDirname(), std::string("testCostDirname"));
}

TEST_F(CostWriterTest, writeCosts) {
    data_ops::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                {"demCom"}, {1}, data_ops::WriteType::STRING,
                                false);

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    data_ops::Matrix4d mat4d({mat3d});
    data_ops::Cost cost;
    cost.fatalOverdoseCost = mat4d;
    cost.healthcareCost = mat4d;
    cost.nonFatalOverdoseCost = mat4d;
    cost.perspective = "perspective";
    cost.pharmaCost = mat4d;
    cost.treatmentCost = mat4d;
    std::string result = writer.writeCosts({cost});

    std::string expected =
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n "
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n";
    EXPECT_EQ(result, expected);
}

TEST_F(CostWriterTest, writeCostsError) {
    data_ops::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                {"demCom"}, {1}, data_ops::WriteType::STRING,
                                false);

    data_ops::Matrix3d mat3d(1, 1, 1);
    std::string result = writer.writeCosts({});

    std::string expected = "failure";
    EXPECT_EQ(result, expected);
}

class UtilityWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UtilityWriterTest, ConstructorEmpty) {
    data_ops::UtilityWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(UtilityWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    data_ops::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   data_ops::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testUtilityDirname"));
}

TEST_F(UtilityWriterTest, writeUtilities) {
    data_ops::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   data_ops::WriteType::STRING, false);

    data_ops::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    data_ops::Matrix4d mat4d({mat3d});
    std::string result = writer.writeUtilities(mat4d);

    std::string expected =
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n";
    EXPECT_EQ(result, expected);
}

TEST_F(UtilityWriterTest, writeUtilitiesError) {
    data_ops::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   data_ops::WriteType::STRING, false);
    data_ops::Matrix4d mat4d;
    std::string result = writer.writeUtilities(mat4d);

    std::string expected = "failure";
    EXPECT_EQ(result, expected);
}

class TotalsWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TotalsWriterTest, ConstructorEmpty) {
    data_ops::TotalsWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(TotalsWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    data_ops::TotalsWriter writer("testTotalsDirname", {"inte"}, {"oud"},
                                  {"dem"}, {"demCom"}, {1},
                                  data_ops::WriteType::STRING);
    EXPECT_EQ(writer.getDirname(), std::string("testTotalsDirname"));
}

TEST_F(TotalsWriterTest, writeUtilities) {
    data_ops::TotalsWriter writer("testTotalsDirname", {"inte"}, {"oud"},
                                  {"dem"}, {"demCom"}, {1},
                                  data_ops::WriteType::STRING);

    data_ops::Totals totals;
    totals.baseCosts = {0.5};
    totals.baseLifeYears = 0.5;
    totals.baseUtility = 0.5;
    totals.discCosts = {0.5};
    totals.discLifeYears = 0.5;
    totals.discUtility = 0.5;
    std::string result = writer.writeTotals(totals);

    std::string expected = "Name, Base, Discount\nPerspective 0, 0.5, "
                           "0.5\nLife Years, 0.5, 0.5\nUtility, 0.5, 0.5\n";
    EXPECT_EQ(result, expected);
}

TEST_F(TotalsWriterTest, writeUtilitiesError) {
    data_ops::TotalsWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                  {"dem"}, {"demCom"}, {1},
                                  data_ops::WriteType::STRING);
    data_ops::Totals totals;
    totals.baseCosts = {};
    totals.baseLifeYears = 0.5;
    totals.baseUtility = 0.5;
    totals.discCosts = {};
    totals.discLifeYears = 0.5;
    totals.discUtility = 0.5;
    std::string result = writer.writeTotals(totals);

    std::string expected = "failure";
    EXPECT_EQ(result, expected);
}
