#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "Writer.hpp"

class WriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(WriterTest, ConstructorStringAndVectorAndWriteType) {
    matrixify::Writer writer("test", {}, matrixify::WriteType::STRING);
    std::string result = writer.getDirname();
    EXPECT_EQ(result, std::string("test"));
}

TEST_F(WriterTest, writeType) {
    matrixify::Writer writer;
    writer.setWriteType(matrixify::WriteType::FILE);
    EXPECT_EQ(writer.getWriteType(), matrixify::WriteType::FILE);
}

TEST_F(WriterTest, dirname) {
    matrixify::Writer writer;
    writer.setDirname("testingDirname");
    EXPECT_EQ(writer.getDirname(), std::string("testingDirname"));
}

TEST_F(WriterTest, timesteps) {
    std::vector<int> timesteps{0, 2, 4, 6, 8};
    matrixify::Writer writer("", timesteps);
    EXPECT_EQ(writer.getTimesteps(), timesteps);
}

class OutputWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(OutputWriterTest, ConstructorEmpty) {
    matrixify::OutputWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(OutputWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    matrixify::OutputWriter writer("testDirname", {"inte"}, {"oud"}, {"dem"},
                                   {"demCom"}, {1},
                                   matrixify::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testDirname"));
}

class HistoryWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HistoryWriterTest, ConstructorEmpty) {
    matrixify::HistoryWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(HistoryWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    matrixify::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    matrixify::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testHistoryDirname"));
}

TEST_F(HistoryWriterTest, writeHistory) {
    matrixify::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    matrixify::WriteType::STRING, false);

    matrixify::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    matrixify::Matrix4d mat4d({mat3d});
    matrixify::History hist;
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
    matrixify::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    matrixify::WriteType::STRING, false);

    matrixify::Matrix3d mat3d(1, 1, 1);
    matrixify::History hist;
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
    matrixify::CostWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(CostWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    matrixify::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                 {"demCom"}, {1}, matrixify::WriteType::STRING,
                                 false);
    EXPECT_EQ(writer.getDirname(), std::string("testCostDirname"));
}

TEST_F(CostWriterTest, writeCosts) {
    matrixify::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                 {"demCom"}, {1}, matrixify::WriteType::STRING,
                                 false);

    matrixify::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    matrixify::Matrix4d mat4d({mat3d});
    matrixify::Cost cost;
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
    matrixify::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                 {"demCom"}, {1}, matrixify::WriteType::STRING,
                                 false);

    matrixify::Matrix3d mat3d(1, 1, 1);
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
    matrixify::UtilityWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(UtilityWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    matrixify::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    matrixify::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testUtilityDirname"));
}

TEST_F(UtilityWriterTest, writeUtilities) {
    matrixify::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    matrixify::WriteType::STRING, false);

    matrixify::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    matrixify::Matrix4d mat4d({mat3d});
    std::string result = writer.writeUtilities(mat4d);

    std::string expected =
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n";
    EXPECT_EQ(result, expected);
}

TEST_F(UtilityWriterTest, writeUtilitiesError) {
    matrixify::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    matrixify::WriteType::STRING, false);
    matrixify::Matrix4d mat4d;
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
    matrixify::TotalsWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(TotalsWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    matrixify::TotalsWriter writer("testTotalsDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   matrixify::WriteType::STRING);
    EXPECT_EQ(writer.getDirname(), std::string("testTotalsDirname"));
}

TEST_F(TotalsWriterTest, writeUtilities) {
    matrixify::TotalsWriter writer("testTotalsDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   matrixify::WriteType::STRING);

    matrixify::Totals totals;
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
    matrixify::TotalsWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   matrixify::WriteType::STRING);
    matrixify::Totals totals;
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
