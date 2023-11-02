#ifndef TESTS_MOCKDATALOADER_HPP_
#define TESTS_MOCKDATALOADER_HPP_

#include "DataLoader.hpp"
#include "gmock/gmock.h"

class MockDataLoader : public Data::IDataLoader {
public:
    MOCK_METHOD(std::string, getDirName, (), (const, override));

    MOCK_METHOD(int, getDuration, (), (const, override));

    MOCK_METHOD(int, getNumOUDStates, (), (const, override));

    MOCK_METHOD(int, getNumInterventions, (), (const, override));

    MOCK_METHOD(int, getNumDemographics, (), (const, override));

    MOCK_METHOD(int, getNumDemographicCombos, (), (const, override));

    MOCK_METHOD(Data::Matrix3d, getInitialSample, (), (const, override));

    MOCK_METHOD(Data::Matrix3dOverTime, getEnteringSamples, (),
                (const, override));

    MOCK_METHOD(Data::Matrix3d, getOUDTransitionRates, (), (const, override));

    MOCK_METHOD(Data::Matrix3dOverTime, getInterventionTransitionRates, (),
                (const, override));

    MOCK_METHOD(Data::Matrix3dOverTime, getOverdoseRates, (),
                (const, override));

    MOCK_METHOD(Data::Matrix3dOverTime, getFatalOverdoseRates, (),
                (const, override));

    MOCK_METHOD(Data::Matrix3d, getMortalityRates, (), (const, override));

    MOCK_METHOD(Data::Matrix3d, getInterventionInitRates, (),
                (const, override));

    MOCK_METHOD(std::vector<std::string>, getInterventions, (),
                (const, override));

    MOCK_METHOD(std::vector<std::string>, getOUDStates, (), (const, override));

    MOCK_METHOD(int, getAgingInterval, (), (const, override));

    MOCK_METHOD(int, getAgeGroupShift, (), (const, override));

    MOCK_METHOD(bool, getCostSwitch, (), (const, override));

    MOCK_METHOD(std::vector<std::string>, getCostPerspectives, (),
                (const, override));

    MOCK_METHOD(double, getDiscountRate, (), (const, override));

    MOCK_METHOD(int, getReportingInterval, (), (const, override));

    MOCK_METHOD(bool, getCostCategoryOutputs, (), (const, override));

    MOCK_METHOD(bool, getPerInterventionPredictions, (), (const, override));

    MOCK_METHOD(bool, getGeneralOutputsSwitch, (), (const, override));

    MOCK_METHOD(std::vector<int>, getGeneralStatsOutputTimesteps, (),
                (const, override));

    MOCK_METHOD(Data::Matrix3d, loadInitialSample, (std::string const &csvName),
                (override));

    MOCK_METHOD(Data::Matrix3dOverTime, loadEnteringSamples,
                (std::string const &csvName,
                 std::string const &enteringSampleIntervention,
                 std::string const &enteringSampleOUD),
                (override));

    MOCK_METHOD(Data::Matrix3d, loadOUDTransitionRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Data::Matrix3d, loadInterventionInitRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Data::Matrix3dOverTime, loadInterventionTransitionRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Data::Matrix3dOverTime, loadOverdoseRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Data::Matrix3dOverTime, loadFatalOverdoseRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Data::Matrix3d, loadMortalityRates,
                (std::string const &smrCSVName, std::string const &bgmCSVName),
                (override));
};

#endif