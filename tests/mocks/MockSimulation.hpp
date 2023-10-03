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

#ifndef TEST_MOCKSIMULATION_HPP_
#define TEST_MOCKSIMULATION_HPP_

#include "DataTypes.hpp"
#include "Simulation.hpp"
#include "gmock/gmock.h"

using namespace Simulation;
using namespace Data;

class MockSimulation : public ISim {
    MOCK_METHOD(void, loadInitialSample, (Matrix3d initialSample), (override));
    MOCK_METHOD(void, loadEnteringSamples, (Matrix3dOverTime enteringSamples),
                (override));
    MOCK_METHOD(void, loadOUDTransitionRates, (Matrix3d oudTransitionRates),
                (override));
    MOCK_METHOD(void, loadInterventionTransitionRates,
                (Matrix3dOverTime InterventionTransitions), (override));
    MOCK_METHOD(void, loadOverdoseRates, (Matrix3dOverTime overdoseRates),
                (override));
    MOCK_METHOD(void, loadMortalityRates, (Matrix3d mortalityRates),
                (override));

    MOCK_METHOD(Matrix3dOverTime, GetEnteringSamples, (), (override));
    MOCK_METHOD(Matrix3d, GetOUDTransitions, (), (override));
    MOCK_METHOD(Matrix3dOverTime, GetInterventionTransitions, (), (override));
    MOCK_METHOD(Matrix3dOverTime, GetOverdoseTransitions, (), (override));
    MOCK_METHOD(Matrix3d, GetMortalityTransitions, (), (override));

    MOCK_METHOD(void, LoadTransitionModules,
                (Matrix3dOverTime enteringSamples, Matrix3d oudTransitionRates,
                 Matrix3d interventionInitRates,
                 Matrix3dOverTime interventionTransitionRates,
                 Matrix3dOverTime fatalOverdoseRates,
                 Matrix3dOverTime overdoseRates, Matrix3d mortalityRates),
                (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(History, getHistory, (), (override));
};

#endif
