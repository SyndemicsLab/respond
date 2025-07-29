////////////////////////////////////////////////////////////////////////////////
// File: data_loader_mock.hpp                                                 //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TESTS_DATALOADERMOCK_HPP_
#define RESPOND_TESTS_DATALOADERMOCK_HPP_

#include <respond/preprocess/data_loader.hpp>

#include <string>

#include <datamanagement/DataManagement.hpp>
#include <gmock/gmock.h>

using namespace respond::preprocess;

class MockDataLoader : public DataLoader {
public:
    MOCK_METHOD(Data::IDataTablePtr, LoadDataTable,
                (const std::string &path, bool headers), (override));

    MOCK_METHOD(void, SetConfig, (const std::string &config_file), (override));

    MOCK_METHOD(Data::IConfigablePtr, GetConfig, (), (const, override));

    MOCK_METHOD(Matrix3d, GetInitialSample, (), (const, override));

    MOCK_METHOD(Matrix3d, GetEnteringSamples, (const int &), (const, override));

    MOCK_METHOD(Matrix3d, GetOUDTransitionRates, (), (const, override));

    MOCK_METHOD(Matrix3d, GetInterventionTransitionRates, (const int &),
                (const, override));

    MOCK_METHOD(Matrix3d, GetOverdoseRates, (const int &), (const, override));

    MOCK_METHOD(Matrix3d, GetFatalOverdoseRates, (const int &),
                (const, override));

    MOCK_METHOD(Matrix3d, GetMortalityRates, (), (const, override));

    MOCK_METHOD(Matrix3d, GetInterventionInitRates, (), (const, override));

    MOCK_METHOD(void, SetInitialSample, (const Matrix3d &), (override));

    MOCK_METHOD(void, SetEnteringSamples, (const TimedMatrix3d &), (override));

    MOCK_METHOD(void, SetOUDTransitionRates, (const Matrix3d &), (override));

    MOCK_METHOD(void, SetInterventionTransitionRates, (const TimedMatrix3d &),
                (override));

    MOCK_METHOD(void, SetOverdoseRates, (const TimedMatrix3d &), (override));

    MOCK_METHOD(void, SetFatalOverdoseRates, (const TimedMatrix3d &),
                (override));

    MOCK_METHOD(void, SetMortalityRates, (const Matrix3d &), (override));

    MOCK_METHOD(void, SetInterventionInitRates, (const Matrix3d &), (override));

    MOCK_METHOD(Matrix3d, LoadInitialSample, (const std::string &), (override));

    MOCK_METHOD(TimedMatrix3d, LoadEnteringSamples,
                (const std::string &, const std::string &, const std::string &),
                (override));

    MOCK_METHOD(TimedMatrix3d, LoadEnteringSamples, (const std::string &),
                (override));

    MOCK_METHOD(Matrix3d, LoadOUDTransitionRates, (const std::string &),
                (override));

    MOCK_METHOD(Matrix3d, LoadInterventionInitRates, (const std::string &),
                (override));

    MOCK_METHOD(TimedMatrix3d, LoadInterventionTransitionRates,
                (const std::string &), (override));

    MOCK_METHOD(TimedMatrix3d, LoadOverdoseRates, (const std::string &),
                (override));

    MOCK_METHOD(TimedMatrix3d, LoadFatalOverdoseRates, (const std::string &),
                (override));

    MOCK_METHOD(Matrix3d, LoadMortalityRates,
                (const std::string &, const std::string &), (override));
};

#endif // RESPOND_TESTS_DATALOADERMOCK_HPP_