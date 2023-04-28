#include <gtest/gtest.h>
#include <iostream>
#include <array>

#include "Configuration.hpp"
#include "DataLoader.hpp"
#include "DataTypes.hpp"
#include "mocks/MockConfiguration.hpp"

using namespace Data;

TEST(DataLoaderTest, DefaultConstructor) {
    DataLoader dataTest;
    std::string EXPECTED = "";
    EXPECT_EQ(dataTest.getDirName(), EXPECTED);
}

TEST(DataLoaderTest, Empty_loadInitialSample) {
    std::string inputDir = "TestInput/input1";
    DataLoader dataloader;
    std::array<long int, 3> order = {0,0,0};
    Data::Matrix3d empty(order);
    empty.setZero();
    Eigen::Tensor<bool, 0> eq = (dataloader.loadInitialSample("init_cohort.csv") == empty).all();
    EXPECT_TRUE(eq(0));
}

