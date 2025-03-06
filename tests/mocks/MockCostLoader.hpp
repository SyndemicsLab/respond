////////////////////////////////////////////////////////////////////////////////
// File: MockCostLoader.hpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef TESTS_MOCKCOSTLOADER_HPP_
#define TESTS_MOCKCOSTLOADER_HPP_

#include "gmock/gmock.h"
#include <respondsimulation/data_ops/CostLoader.hpp>

class MockCostLoader : public data_ops::ICostLoader {
public:
    // ILoggable
    MOCK_METHOD(void, setLogger, ((std::shared_ptr<spdlog::logger> const)),
                (override));
    MOCK_METHOD((std::shared_ptr<spdlog::logger>), getLogger, (),
                (const, override));

    // IOutputable
    MOCK_METHOD(bool, getPerInterventionPredictions, (), (const, override));
    MOCK_METHOD(bool, getGeneralOutputsSwitch, (), (const, override));
    MOCK_METHOD((std::vector<int>), getGeneralStatsOutputTimesteps, (),
                (const, override));

    // IConfigable
    MOCK_METHOD(bool, loadConfigFile, ((std::string const &)), (override));
    MOCK_METHOD(bool, loadConfigPtr, ((Data::IConfigablePtr)), (override));
    MOCK_METHOD((Data::IConfigablePtr), getConfig, (), (const, override));

    // ISimulatable
    MOCK_METHOD((int), getDuration, (), (const, override));
    MOCK_METHOD((bool), getEnteringCohortToggle, (), (const, override));

    // ITableable
    MOCK_METHOD((Data::IDataTablePtr), readCSV, (std::string const &, bool),
                (override));
    MOCK_METHOD((std::unordered_map<std::string, Data::IDataTablePtr>),
                readInputDir, ((std::string const &)), (override));
    MOCK_METHOD((Data::IDataTablePtr), loadTable, ((std::string const &)),
                (override));

    // IIntervention
    MOCK_METHOD((std::vector<std::string>), getInterventions, (),
                (const, override));
    MOCK_METHOD((int), getNumInterventions, (), (const, override));
    MOCK_METHOD((std::vector<int>), getInterventionChangeTimes, (),
                (const, override));
    MOCK_METHOD((void), setInterventions, (const std::vector<std::string>),
                (override));
    MOCK_METHOD((void), setInterventionChangeTimes, (const std::vector<int>),
                (override));

    // IBehavior
    MOCK_METHOD((std::vector<std::string>), getOUDStates, (),
                (const, override));
    MOCK_METHOD((int), getNumOUDStates, (), (const, override));
    MOCK_METHOD((void), setOUDStates, (const std::vector<std::string>),
                (override));

    // IDemographics
    MOCK_METHOD((std::vector<std::string>), getDemographics, (),
                (const, override));
    MOCK_METHOD((int), getNumDemographics, (), (const, override));
    MOCK_METHOD((std::vector<std::string>), getDemographicCombos, (),
                (const, override));
    MOCK_METHOD((int), getNumDemographicCombos, (), (const, override));
    MOCK_METHOD((void), setDemographics, (const std::vector<std::string>),
                (override));
    MOCK_METHOD((void), setDemographicCombos, (const std::vector<std::string>),
                (override));

    MOCK_METHOD((int), getAgeGroupShift, (), (const, override));
    MOCK_METHOD((int), getAgingInterval, (), (const, override));
    MOCK_METHOD((std::vector<std::string>), getAgeGroupBins, (),
                (const, override));
    MOCK_METHOD((std::vector<int>), getEnteringSampleChangeTimes, (),
                (const, override));
    MOCK_METHOD((std::vector<int>), getOverdoseChangeTimes, (),
                (const, override));

    // ICosting
    MOCK_METHOD((bool), getCostSwitch, (), (const, override));
    MOCK_METHOD((std::vector<std::string>), getCostPerspectives, (),
                (const, override));
    MOCK_METHOD((double), getDiscountRate, (), (const, override));
    MOCK_METHOD((bool), getCostCategoryOutputs, (), (const, override));
    MOCK_METHOD((std::vector<int>), getCostUtilityOutputTimesteps, (),
                (const, override));

    // ICostLoader
    MOCK_METHOD((std::unordered_map<std::string, data_ops::Matrix3d>),
                loadHealthcareUtilizationCost, (std::string const &),
                (override));
    MOCK_METHOD((std::unordered_map<std::string,
                                    std::unordered_map<std::string, double>>),
                loadOverdoseCost, (std::string const &), (override));
    MOCK_METHOD((std::unordered_map<std::string, data_ops::Matrix3d>),
                loadPharmaceuticalCost, (std::string const &), (override));
    MOCK_METHOD((std::unordered_map<std::string, data_ops::Matrix3d>),
                loadTreatmentUtilizationCost, (std::string const &),
                (override));
    MOCK_METHOD((data_ops::Matrix3d), getHealthcareUtilizationCost,
                (std::string const &), (const, override));
    MOCK_METHOD((data_ops::Matrix3d), getPharmaceuticalCost,
                (std::string const &), (const, override));
    MOCK_METHOD((data_ops::Matrix3d), getTreatmentUtilizationCost,
                (std::string const &), (const, override));
    MOCK_METHOD((double), getNonFatalOverdoseCost, (std::string const &),
                (const, override));
    MOCK_METHOD((double), getFatalOverdoseCost, (std::string const &),
                (const, override));
};

#endif