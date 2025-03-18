////////////////////////////////////////////////////////////////////////////////
// File: cost_loader_mock.hpp                                                 //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TESTS_COSTLOADERMOCK_HPP_
#define RESPOND_TESTS_COSTLOADERMOCK_HPP_

#include <respond/data_ops/cost_loader.hpp>

#include <string>
#include <unordered_map>

#include <gmock/gmock.h>

#include <respond/data_ops/matrices.hpp>

using namespace respond::data_ops;

class MockCostLoader : public virtual CostLoader {
public:
    MOCK_METHOD((std::unordered_map<std::string, Matrix3d>),
                LoadHealthcareUtilizationCost, ((const std::string &)),
                (override));

    MOCK_METHOD((std::unordered_map<std::string,
                                    std::unordered_map<std::string, double>>),
                LoadOverdoseCost, ((const std::string &)), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrix3d>),
                LoadPharmaceuticalCost, ((const std::string &)), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrix3d>),
                LoadTreatmentUtilizationCost, ((const std::string &)),
                (override));

    MOCK_METHOD((Matrix3d), GetHealthcareUtilizationCost,
                ((const std::string &)), (const, override));

    MOCK_METHOD((Matrix3d), GetPharmaceuticalCost, ((const std::string &)),
                (const, override));

    MOCK_METHOD((Matrix3d), GetTreatmentUtilizationCost,
                ((const std::string &)), (const, override));

    MOCK_METHOD((double), GetNonFatalOverdoseCost, ((const std::string &)),
                (const, override));

    MOCK_METHOD((double), GetFatalOverdoseCost, ((const std::string &)),
                (const, override));
};

#endif // RESPOND_TESTS_COSTLOADERMOCK_HPP_