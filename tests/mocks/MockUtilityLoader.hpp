#ifndef TESTS_MOCKUTILITYLOADER_HPP_
#define TESTS_MOCKUTILITYLOADER_HPP_

#include "Interfaces.hpp"
#include "gmock/gmock.h"

class MockUtilityLoader : public Matrixify::IUtilityLoader {
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
    MOCK_METHOD((Data::IDataTablePtr), readCSV, ((std::string const &)),
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

    // IBehavior
    MOCK_METHOD((std::vector<std::string>), getOUDStates, (),
                (const, override));
    MOCK_METHOD((int), getNumOUDStates, (), (const, override));

    // IDemographics
    MOCK_METHOD((std::vector<std::string>), getDemographics, (),
                (const, override));
    MOCK_METHOD((int), getNumDemographics, (), (const, override));
    MOCK_METHOD((std::vector<std::string>), getDemographicCombos, (),
                (const, override));
    MOCK_METHOD((int), getNumDemographicCombos, (), (const, override));
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

    // IUtilityLoader
    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadBackgroundUtility, (std::string const &), (override));
    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadOUDUtility, (std::string const &), (override));
    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadSettingUtility, (std::string const &), (override));
    MOCK_METHOD((Matrixify::Matrix3d), getBackgroundUtility,
                (std::string const &), (const, override));
    MOCK_METHOD((Matrixify::Matrix3d), getOUDUtility, (std::string const &),
                (const, override));
    MOCK_METHOD((Matrixify::Matrix3d), getSettingUtility, (std::string const &),
                (const, override));
};

#endif