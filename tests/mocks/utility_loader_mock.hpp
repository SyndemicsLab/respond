////////////////////////////////////////////////////////////////////////////////
// File: utility_loader_mock.hpp                                              //
// Project: mocks                                                             //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TESTS_UTILITYLOADERMOCK_HPP_
#define RESPOND_TESTS_UTILITYLOADERMOCK_HPP_

#include <respond/data_ops/utility_loader.hpp>

#include <string>
#include <unordered_map>

#include <gmock/gmock.h>

#include <respond/data_ops/matrices.hpp>

using namespace respond::data_ops;

class MockUtilityLoader : public UtilityLoader {
public:
    MOCK_METHOD(Data::IDataTablePtr, LoadDataTable,
                (const std::string &path, bool headers), (override));

    MOCK_METHOD(void, SetConfig, (const std::string &config_file), (override));

    MOCK_METHOD(Data::IConfigablePtr, GetConfig, (), (const, override));

    MOCK_METHOD((StringUOMap<Matrix3d>), LoadBackgroundUtility,
                (const std::string &), (override));

    MOCK_METHOD((StringUOMap<Matrix3d>), LoadOUDUtility, (const std::string &),
                (override));

    MOCK_METHOD((StringUOMap<Matrix3d>), LoadSettingUtility,
                (const std::string &), (override));

    MOCK_METHOD(Matrix3d, GetBackgroundUtility, (const std::string &),
                (const, override));

    MOCK_METHOD(Matrix3d, GetOUDUtility, (const std::string &),
                (const, override));

    MOCK_METHOD(Matrix3d, GetSettingUtility, (const std::string &),
                (const, override));
};

#endif // RESPOND_TESTS_UTILITYLOADERMOCK_HPP_