//===-----------------------------*- C++ -*--------------------------------===//
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

#include <gtest/gtest.h>

#include "PostSimulationCalculator.hpp"

class PostSimulationCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PostSimulationCalculatorTest, constructor) {}

TEST_F(PostSimulationCalculatorTest, calculateCost) {}

TEST_F(PostSimulationCalculatorTest, calculateUtility) {}
