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
