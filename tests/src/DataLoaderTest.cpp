//===-- DataLoaderTest.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:33 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <array>
#include <gtest/gtest.h>
#include <iostream>

#include "../mocks/MockConfiguration.hpp"
#include "Configuration.hpp"
#include "DataLoader.hpp"
#include "DataTypes.hpp"

using namespace Data;

TEST(DataLoaderTest, DefaultConstructor) {
    DataLoader dataTest;
    std::string EXPECTED = "";
    EXPECT_EQ(dataTest.getDirName(), EXPECTED);
}
