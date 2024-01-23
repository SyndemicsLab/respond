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

class MockSimulation : public ISim {
    MOCK_METHOD(void, loadInitialSample,
                (Matrixify::Matrix3d const &initialSample), (override));
    MOCK_METHOD(void, loadEnteringSamples,
                (Matrixify::Matrix3dOverTime const &enteringSamples),
                (override));
    MOCK_METHOD(void, loadOUDTransitionRates,
                (Matrixify::Matrix3d const &oudTransitionRates), (override));
    MOCK_METHOD(void, loadInterventionTransitionRates,
                (Matrixify::Matrix3dOverTime const &InterventionTransitions),
                (override));
    MOCK_METHOD(void, loadOverdoseRates,
                (Matrixify::Matrix3dOverTime const &overdoseRates), (override));
    MOCK_METHOD(void, loadMortalityRates,
                (Matrixify::Matrix3d const &mortalityRates), (override));

    MOCK_METHOD(Matrixify::Matrix3dOverTime, GetEnteringSamples, (),
                (const, override));
    MOCK_METHOD(Matrixify::Matrix3d, GetOUDTransitions, (), (const, override));
    MOCK_METHOD(Matrixify::Matrix3dOverTime, GetInterventionTransitions, (),
                (const, override));
    MOCK_METHOD(Matrixify::Matrix3dOverTime, GetOverdoseTransitions, (),
                (const, override));
    MOCK_METHOD(Matrixify::Matrix3d, GetMortalityTransitions, (),
                (const, override));

    MOCK_METHOD(void, LoadTransitionModules,
                (Matrixify::Matrix3dOverTime const &enteringSamples,
                 Matrixify::Matrix3d const &oudTransitionRates,
                 Matrixify::Matrix3d const &interventionInitRates,
                 Matrixify::Matrix3dOverTime const &interventionTransitionRates,
                 Matrixify::Matrix3dOverTime const &fatalOverdoseRates,
                 Matrixify::Matrix3dOverTime const &overdoseRates,
                 Matrixify::Matrix3d const &mortalityRates),
                (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(Matrixify::History, getHistory, (), (const, override));
};

#endif
