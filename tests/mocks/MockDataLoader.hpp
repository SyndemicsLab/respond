#ifndef TESTS_MOCKDATALOADER_HPP_
#define TESTS_MOCKDATALOADER_HPP_

#include "DataLoader.hpp"
#include "gmock/gmock.h"

class MockDataLoader : public matrixify::IDataLoader {
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

    // IDataLoader
    MOCK_METHOD((matrixify::Matrix3d), getInitialSample, (), (const, override));
    MOCK_METHOD((matrixify::Matrix4d), getEnteringSamples, (),
                (const, override));
    MOCK_METHOD((matrixify::Matrix3d), getOUDTransitionRates, (),
                (const, override));
    MOCK_METHOD((matrixify::Matrix4d), getInterventionTransitionRates, (),
                (const, override));
    MOCK_METHOD((matrixify::Matrix4d), getOverdoseRates, (), (const, override));
    MOCK_METHOD((matrixify::Matrix4d), getFatalOverdoseRates, (),
                (const, override));
    MOCK_METHOD((matrixify::Matrix3d), getMortalityRates, (),
                (const, override));
    MOCK_METHOD((matrixify::Matrix3d), getInterventionInitRates, (),
                (const, override));
    MOCK_METHOD((void), setInitialSample, ((matrixify::Matrix3d)), (override));
    MOCK_METHOD((void), setEnteringSamples, ((matrixify::Matrix4d)),
                (override));
    MOCK_METHOD((void), setOUDTransitionRates, ((matrixify::Matrix3d)),
                (override));
    MOCK_METHOD((void), setInterventionTransitionRates, ((matrixify::Matrix4d)),
                (override));
    MOCK_METHOD((void), setOverdoseRates, ((matrixify::Matrix4d)), (override));
    MOCK_METHOD((void), setFatalOverdoseRates, ((matrixify::Matrix4d)),
                (override));
    MOCK_METHOD((void), setMortalityRates, ((matrixify::Matrix3d)), (override));
    MOCK_METHOD((void), setInterventionInitRates, ((matrixify::Matrix3d)),
                (override));
    MOCK_METHOD((matrixify::Matrix3d), loadInitialSample,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix4d), loadEnteringSamples,
                ((std::string const &), (std::string const &),
                 (std::string const &)),
                (override));
    MOCK_METHOD((matrixify::Matrix4d), loadEnteringSamples,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix3d), loadOUDTransitionRates,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix3d), loadInterventionInitRates,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix4d), loadInterventionTransitionRates,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix4d), loadOverdoseRates,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix4d), loadFatalOverdoseRates,
                ((std::string const &)), (override));
    MOCK_METHOD((matrixify::Matrix3d), loadMortalityRates,
                ((std::string const &), (std::string const &)), (override));
};

#endif