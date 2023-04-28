#ifndef TEST_MOCKCONFIGURATION_HPP_
#define TEST_MOCKCONFIGURATION_HPP_

#include "gmock/gmock.h"
#include "Configuration.hpp"
#include "DataTypes.hpp"

using namespace Data;

class MockConfiguration : public IConfiguration {
    MOCK_METHOD(std::vector<std::string>, getInterventions, (), (override));
    MOCK_METHOD(std::vector<std::string>, getOUDStates, (), (override));
    MOCK_METHOD(int, getNumDemographicCombos, (), (override));
    MOCK_METHOD(std::vector<int>, getDemographicCounts, (), (override));
    MOCK_METHOD(std::vector<int>, getEnteringSampleChangeTimes, (), (override));
    MOCK_METHOD(std::vector<int>, getInterventionChangeTimes, (), (override));
    MOCK_METHOD(std::vector<int>, getOverdoseChangeTimes, (), (override));
    MOCK_METHOD(std::vector<std::string>, getDemographicCombos, (), (override));
    MOCK_METHOD(int, getAgingInterval, (), (override));
    MOCK_METHOD(int, getDuration, (), (override));
};

#endif
