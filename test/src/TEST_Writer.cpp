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
    Matrixify::Writer writer("test", {}, Matrixify::WriteType::STRING);
    std::string result = writer.getDirname();
    EXPECT_EQ(result, std::string("test"));
}

TEST_F(WriterTest, writeType) {
    Matrixify::Writer writer;
    writer.setWriteType(Matrixify::WriteType::FILE);
    EXPECT_EQ(writer.getWriteType(), Matrixify::WriteType::FILE);
}

TEST_F(WriterTest, dirname) {
    Matrixify::Writer writer;
    writer.setDirname("testingDirname");
    EXPECT_EQ(writer.getDirname(), std::string("testingDirname"));
}

TEST_F(WriterTest, timesteps) {
    std::vector<int> timesteps{0, 2, 4, 6, 8};
    Matrixify::Writer writer("", timesteps);
    EXPECT_EQ(writer.getTimesteps(), timesteps);
}

class OutputWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(OutputWriterTest, ConstructorEmpty) {
    Matrixify::OutputWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(OutputWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    Matrixify::OutputWriter writer("testDirname", {"inte"}, {"oud"}, {"dem"},
                                   {"demCom"}, {1},
                                   Matrixify::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testDirname"));
}

class HistoryWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HistoryWriterTest, ConstructorEmpty) {
    Matrixify::HistoryWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(HistoryWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    Matrixify::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    Matrixify::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testHistoryDirname"));
}

TEST_F(HistoryWriterTest, writeHistory) {
    Matrixify::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    Matrixify::WriteType::STRING, false);

    Matrixify::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    Matrixify::Matrix4d mat4d({mat3d});
    Matrixify::History hist;
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
    Matrixify::HistoryWriter writer("testHistoryDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    Matrixify::WriteType::STRING, false);

    Matrixify::Matrix3d mat3d(1, 1, 1);
    Matrixify::History hist;
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
    Matrixify::CostWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(CostWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    Matrixify::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                 {"demCom"}, {1}, Matrixify::WriteType::STRING,
                                 false);
    EXPECT_EQ(writer.getDirname(), std::string("testCostDirname"));
}

TEST_F(CostWriterTest, writeCosts) {
    Matrixify::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                 {"demCom"}, {1}, Matrixify::WriteType::STRING,
                                 false);

    Matrixify::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    Matrixify::Matrix4d mat4d({mat3d});
    Matrixify::Cost cost;
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
    Matrixify::CostWriter writer("testCostDirname", {"inte"}, {"oud"}, {"dem"},
                                 {"demCom"}, {1}, Matrixify::WriteType::STRING,
                                 false);

    Matrixify::Matrix3d mat3d(1, 1, 1);
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
    Matrixify::UtilityWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(UtilityWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    Matrixify::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    Matrixify::WriteType::STRING, false);
    EXPECT_EQ(writer.getDirname(), std::string("testUtilityDirname"));
}

TEST_F(UtilityWriterTest, writeUtilities) {
    Matrixify::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    Matrixify::WriteType::STRING, false);

    Matrixify::Matrix3d mat3d(1, 1, 1);
    mat3d(0, 0, 0) = 0.25;
    Matrixify::Matrix4d mat4d({mat3d});
    std::string result = writer.writeUtilities(mat4d);

    std::string expected =
        "Interventions,OUD States,dem,t+1,\ninte,oud,demCom,0.250000,\n";
    EXPECT_EQ(result, expected);
}

TEST_F(UtilityWriterTest, writeUtilitiesError) {
    Matrixify::UtilityWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                    {"dem"}, {"demCom"}, {1},
                                    Matrixify::WriteType::STRING, false);
    Matrixify::Matrix4d mat4d;
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
    Matrixify::TotalsWriter writer;
    EXPECT_EQ(writer.getDirname(), std::string(""));
}

TEST_F(TotalsWriterTest, ConstructorStringAndVectorsAndWriteTypeAndBool) {
    Matrixify::TotalsWriter writer("testTotalsDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   Matrixify::WriteType::STRING);
    EXPECT_EQ(writer.getDirname(), std::string("testTotalsDirname"));
}

TEST_F(TotalsWriterTest, writeUtilities) {
    Matrixify::TotalsWriter writer("testTotalsDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   Matrixify::WriteType::STRING);

    Matrixify::Totals totals;
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
    Matrixify::TotalsWriter writer("testUtilityDirname", {"inte"}, {"oud"},
                                   {"dem"}, {"demCom"}, {1},
                                   Matrixify::WriteType::STRING);
    Matrixify::Totals totals;
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
