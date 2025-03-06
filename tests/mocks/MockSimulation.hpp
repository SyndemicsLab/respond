////////////////////////////////////////////////////////////////////////////////
// File: MockSimulation.hpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef TESTS_MOCKSIMULATION_HPP_
#define TESTS_MOCKSIMULATION_HPP_

#include "gmock/gmock.h"
#include <respondsimulation/data_ops/DataTypes.hpp>
#include <respondsimulation/model/Simulation.hpp>

using namespace Simulation;

class MockSimulation : public IRespond {
    MOCK_METHOD((data_ops::Matrix3d), getState, (), (const, override));
    MOCK_METHOD((std::shared_ptr<data_ops::IDataLoader>), getDataLoader, (),
                (const, override));
    MOCK_METHOD((std::shared_ptr<data_ops::ICostLoader>), getCostLoader, (),
                (const, override));
    MOCK_METHOD((std::shared_ptr<data_ops::IUtilityLoader>), getUtilityLoader,
                (), (const, override));
    MOCK_METHOD(void, ageUp, (), (override));
    MOCK_METHOD((data_ops::History), getHistory, (), (const, override));

    MOCK_METHOD(void, run,
                ((std::shared_ptr<data_ops::IDataLoader> const &),
                 (std::shared_ptr<data_ops::ICostLoader> const &),
                 (std::shared_ptr<data_ops::IUtilityLoader> const &)),
                (override));
    MOCK_METHOD(std::uint64_t, getCurrentTime, (), (const, override));
    MOCK_METHOD(void, setDuration, (std::uint64_t const), (override));
    MOCK_METHOD(std::uint64_t, getDuration, (), (const, override));

    MOCK_METHOD(void, setData,
                ((std::shared_ptr<data_ops::IDataLoader> const &)), (override));
    MOCK_METHOD(void, setCost,
                ((std::shared_ptr<data_ops::ICostLoader> const &)), (override));
    MOCK_METHOD(void, setUtility,
                ((std::shared_ptr<data_ops::IUtilityLoader> const &)),
                (override));
};

#endif
