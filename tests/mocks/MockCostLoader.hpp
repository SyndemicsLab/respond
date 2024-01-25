#ifndef TESTS_MOCKCOSTLOADER_HPP_
#define TESTS_MOCKCOSTLOADER_HPP_

#include "CostLoader.hpp"
#include "gmock/gmock.h"

#include <DataManagement.hpp>

class MockCostLoader : public Matrixify::ICostLoader {
public:
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

    MOCK_METHOD(void, populateCostParameters, (), (override));

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

    MOCK_METHOD((std::vector<std::string>), getCostPerspectives, (),
                (const, override));

    MOCK_METHOD(bool, getCostSwitch, (), (const, override));

    MOCK_METHOD(double, getDiscountRate, (), (const, override));

    MOCK_METHOD(Matrixify::Configuration, loadConfigurationFile,
                (std::string const &configPath), (override));

    MOCK_METHOD(Matrixify::Configuration, readConfigFile, (std::string const &),
                (override));

    MOCK_METHOD(Data::IDataTablePtr, readCSV, (std::string const &),
                (override));

    MOCK_METHOD((std::unordered_map<std::string, Data::IDataTablePtr>),
                readInputDir, (std::string const &), (override));

    MOCK_METHOD(Matrixify::Configuration, getConfiguration, (),
                (const, override));

    MOCK_METHOD(Data::IDataTablePtr, loadTable, (std::string const &filename),
                (override));
};

#endif