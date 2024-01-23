//===-- MockConfiguration.hpp - Instruction class definition ----*- C++ -*-===//
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

#ifndef TESTS_MOCKCONFIGURATION_HPP_
#define TESTS_MOCKCONFIGURATION_HPP_

#include "Configuration.hpp"
#include "DataTypes.hpp"
#include "gmock/gmock.h"

using namespace Matrixify;

class MockConfiguration : public IConfiguration {
    MOCK_METHOD(std::vector<std::string>, getInterventions, (), (override));
    MOCK_METHOD(std::vector<std::string>, getOUDStates, (), (override));
    MOCK_METHOD(int, getNumDemographicCombos, (), (override));
    MOCK_METHOD(std::vector<int>, getDemographicCounts, (), (override));
    MOCK_METHOD(std::vector<int>, getEnteringSampleChangeTimes, (), (override));
    MOCK_METHOD(std::vector<int>, getInterventionChangeTimes, (), (override));
    MOCK_METHOD(std::vector<int>, getOverdoseChangeTimes, (), (override));
    MOCK_METHOD(std::vector<std::string>, getDemographicCombos, (), (override));
    MOCK_METHOD(int, getAgingInterval, (), (override));
    MOCK_METHOD(int, getDuration, (), (override));
};

#endif
