////////////////////////////////////////////////////////////////////////////////
// File: utility_loader_mock.hpp                                              //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-18                                                  //
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

class MockUtilityLoader : public virtual UtilityLoader {
public:
    MOCK_METHOD((std::unordered_map<std::string, Matrix3d>),
                LoadBackgroundUtility, (const std::string &), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrix3d>), LoadOUDUtility,
                (const std::string &), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrix3d>), LoadSettingUtility,
                (const std::string &), (override));

    MOCK_METHOD(Matrix3d, GetBackgroundUtility, (const std::string &),
                (const, override));

    MOCK_METHOD(Matrix3d, GetOUDUtility, (const std::string &),
                (const, override));

    MOCK_METHOD(Matrix3d, GetSettingUtility, (const std::string &),
                (const, override));
};

#endif // RESPOND_TESTS_UTILITYLOADERMOCK_HPP_