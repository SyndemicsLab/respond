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
