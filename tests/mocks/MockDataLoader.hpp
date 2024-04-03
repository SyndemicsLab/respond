#ifndef TESTS_MOCKDATALOADER_HPP_
#define TESTS_MOCKDATALOADER_HPP_

#include "DataLoader.hpp"
#include "gmock/gmock.h"

class MockDataLoader : public Matrixify::IDataLoader {
public:
    MOCK_METHOD(int, getDuration, (), (const, override));

    MOCK_METHOD(int, getNumOUDStates, (), (const, override));

    MOCK_METHOD(int, getNumInterventions, (), (const, override));

    MOCK_METHOD(int, getNumDemographics, (), (const, override));

    MOCK_METHOD(int, getNumDemographicCombos, (), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getInitialSample, (), (const, override));

    MOCK_METHOD(Matrixify::Matrix4d, getEnteringSamples, (), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getOUDTransitionRates, (),
                (const, override));

    MOCK_METHOD(Matrixify::Matrix4d, getInterventionTransitionRates, (),
                (const, override));

    MOCK_METHOD(Matrixify::Matrix4d, getOverdoseRates, (), (const, override));

    MOCK_METHOD(Matrixify::Matrix4d, getFatalOverdoseRates, (),
                (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getMortalityRates, (), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getInterventionInitRates, (),
                (const, override));

    MOCK_METHOD(std::vector<std::string>, getInterventions, (),
                (const, override));

    MOCK_METHOD(std::vector<std::string>, getOUDStates, (), (const, override));

    MOCK_METHOD(void, setInitialSample, (Matrixify::Matrix3d mat), (override));

    MOCK_METHOD(void, setEnteringSamples, (Matrixify::Matrix4d mat),
                (override));

    MOCK_METHOD(void, setOUDTransitionRates, (Matrixify::Matrix3d mat),
                (override));

    MOCK_METHOD(void, setInterventionTransitionRates, (Matrixify::Matrix4d mat),
                (override));

    MOCK_METHOD(void, setOverdoseRates, (Matrixify::Matrix4d mat), (override));

    MOCK_METHOD(void, setFatalOverdoseRates, (Matrixify::Matrix4d mat),
                (override));

    MOCK_METHOD(void, setMortalityRates, (Matrixify::Matrix3d mat), (override));

    MOCK_METHOD(void, setInterventionInitRates, (Matrixify::Matrix3d mat),
                (override));

    MOCK_METHOD(int, getAgingInterval, (), (const, override));

    MOCK_METHOD(int, getAgeGroupShift, (), (const, override));

    MOCK_METHOD(bool, getCostSwitch, (), (const, override));

    MOCK_METHOD(std::vector<std::string>, getCostPerspectives, (),
                (const, override));

    MOCK_METHOD(double, getDiscountRate, (), (const, override));

    MOCK_METHOD(std::vector<int>, getCostUtilityOutputTimesteps, (),
                (const, override));

    MOCK_METHOD(bool, getCostCategoryOutputs, (), (const, override));

    MOCK_METHOD(bool, getPerInterventionPredictions, (), (const, override));

    MOCK_METHOD(bool, getGeneralOutputsSwitch, (), (const, override));

    MOCK_METHOD(std::vector<int>, getGeneralStatsOutputTimesteps, (),
                (const, override));

    MOCK_METHOD((std::shared_ptr<spdlog::logger>), getLogger, (),
                (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, loadInitialSample,
                (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix4d, loadEnteringSamples,
                (std::string const &csvName,
                 std::string const &enteringSampleIntervention,
                 std::string const &enteringSampleOUD),
                (override));

    MOCK_METHOD(Matrixify::Matrix3d, loadOUDTransitionRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix3d, loadInterventionInitRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix4d, loadInterventionTransitionRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix4d, loadOverdoseRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix4d, loadFatalOverdoseRates,
                (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix3d, loadMortalityRates,
                (std::string const &smrCSVName, std::string const &bgmCSVName),
                (override));
};

#endif