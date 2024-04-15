#ifndef TESTS_MOCKCOSTLOADER_HPP_
#define TESTS_MOCKCOSTLOADER_HPP_

#include "CostLoader.hpp"
#include "gmock/gmock.h"

#include <DataManagement.hpp>

class MockCostLoader : public Matrixify::ICostLoader {
public:
    MOCK_METHOD(bool, loadConfigFile, (std::string const &configPath),
                (override));

    MOCK_METHOD(bool, loadConfigPtr, (Data::IConfigablePtr configPtr),
                (override));

    MOCK_METHOD((Data::IDataTablePtr), readCSV, (std::string const &),
                (override));

    MOCK_METHOD((std::unordered_map<std::string, Data::IDataTablePtr>),
                readInputDir, (std::string const &), (override));

    MOCK_METHOD(Data::IConfigablePtr, getConfig, (), (const, override));

    MOCK_METHOD((Data::IDataTablePtr), loadTable, (std::string const &filename),
                (override));

    MOCK_METHOD(int, getDuration, (), (const, override));

    MOCK_METHOD(int, getAgingInterval, (), (const, override));

    MOCK_METHOD((std::vector<int>), getInterventionChangeTimes, (),
                (const, override));

    MOCK_METHOD((std::vector<int>), getEnteringSampleChangeTimes, (),
                (const, override));

    MOCK_METHOD((std::vector<int>), getOverdoseChangeTimes, (),
                (const, override));

    MOCK_METHOD((std::vector<std::string>), getInterventions, (),
                (const, override));

    MOCK_METHOD((std::vector<std::string>), getOUDStates, (),
                (const, override));

    MOCK_METHOD(int, getNumOUDStates, (), (const, override));

    MOCK_METHOD(int, getNumInterventions, (), (const, override));

    MOCK_METHOD((std::vector<std::string>), getDemographics, (),
                (const, override));

    MOCK_METHOD(int, getNumDemographics, (), (const, override));

    MOCK_METHOD((std::vector<std::string>), getDemographicCombos, (),
                (const, override));

    MOCK_METHOD(int, getNumDemographicCombos, (), (const, override));

    MOCK_METHOD(int, getAgeGroupShift, (), (const, override));

    MOCK_METHOD(bool, getCostSwitch, (), (const, override));

    MOCK_METHOD((std::vector<std::string>), getCostPerspectives, (),
                (const, override));

    MOCK_METHOD(double, getDiscountRate, (), (const, override));

    MOCK_METHOD(bool, getCostCategoryOutputs, (), (const, override));

    MOCK_METHOD((std::vector<int>), getCostUtilityOutputTimesteps, (),
                (const, override));

    MOCK_METHOD(bool, getPerInterventionPredictions, (), (const, override));

    MOCK_METHOD(bool, getGeneralOutputsSwitch, (), (const, override));

    MOCK_METHOD((std::vector<int>), getGeneralStatsOutputTimesteps, (),
                (const, override));

    MOCK_METHOD((std::shared_ptr<spdlog::logger>), getLogger, (),
                (const, override));

    MOCK_METHOD((void), setLogger,
                (std::shared_ptr<spdlog::logger> const logger), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadHealthcareUtilizationCost, (std::string const &csvName),
                (override));

    MOCK_METHOD((std::unordered_map<std::string,
                                    std::unordered_map<std::string, double>>),
                loadOverdoseCost, (std::string const &csvName), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadPharmaceuticalCost, (std::string const &csvName),
                (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadTreatmentUtilizationCost, (std::string const &csvName),
                (override));

    MOCK_METHOD(Matrixify::Matrix3d, getHealthcareUtilizationCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getPharmaceuticalCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getTreatmentUtilizationCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(double, getNonFatalOverdoseCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(double, getFatalOverdoseCost, (std::string const &perspective),
                (const, override));
};

#endif