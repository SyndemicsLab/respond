//===-- MockSimulation.hpp - Instruction class definition -------*- C++ -*-===//
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

#ifndef TESTS_MOCKSIMULATION_HPP_
#define TESTS_MOCKSIMULATION_HPP_

#include "DataTypes.hpp"
#include "Simulation.hpp"
#include "gmock/gmock.h"

using namespace Simulation;

class MockSimulation : public IRespond {
    MOCK_METHOD((Matrixify::Matrix3d), getState, (), (const, override));
    MOCK_METHOD((std::shared_ptr<Matrixify::IDataLoader>), getDataLoader, (),
                (const, override));
    MOCK_METHOD((std::shared_ptr<Matrixify::ICostLoader>), getCostLoader, (),
                (const, override));
    MOCK_METHOD((std::shared_ptr<Matrixify::IUtilityLoader>), getUtilityLoader,
                (), (const, override));
    MOCK_METHOD(void, ageUp, (), (override));
    MOCK_METHOD((Matrixify::History), getHistory, (), (const, override));

    MOCK_METHOD(void, run,
                ((std::shared_ptr<Matrixify::IDataLoader> const &),
                 (std::shared_ptr<Matrixify::ICostLoader> const &),
                 (std::shared_ptr<Matrixify::IUtilityLoader> const &)),
                (override));
    MOCK_METHOD(std::uint64_t, getCurrentTime, (), (const, override));
    MOCK_METHOD(void, setDuration, (std::uint64_t const), (override));
    MOCK_METHOD(std::uint64_t, getDuration, (), (const, override));

    MOCK_METHOD(void, setData,
                ((std::shared_ptr<Matrixify::IDataLoader> const &)),
                (override));
    MOCK_METHOD(void, setCost,
                ((std::shared_ptr<Matrixify::ICostLoader> const &)),
                (override));
    MOCK_METHOD(void, setUtility,
                ((std::shared_ptr<Matrixify::IUtilityLoader> const &)),
                (override));
};

#endif
