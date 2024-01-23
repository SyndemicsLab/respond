#ifndef TESTS_MOCKUTILITYLOADER_HPP_
#define TESTS_MOCKUTILITYLOADER_HPP_

#include "UtilityLoader.hpp"
#include "gmock/gmock.h"

class MockUtilityLoader : public Matrixify::IUtilityLoader {
public:
    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadBackgroundUtility, (std::string const &csvName),
                (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadOUDUtility, (std::string const &csvName), (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrixify::Matrix3d>),
                loadSettingUtility, (std::string const &csvName), (override));

    MOCK_METHOD(Matrixify::Matrix3d, getBackgroundUtility,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getOUDUtility,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(Matrixify::Matrix3d, getSettingUtility,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(double, getDiscountRate, (), (const, override));

    MOCK_METHOD(Matrixify::Configuration, loadConfigurationFile,
                (std::string const &configPath), (override));

    MOCK_METHOD(Matrixify::Configuration, readConfigFile, (std::string const &),
                (override));

    MOCK_METHOD(Matrixify::InputTable, readCSV, (std::string const &),
                (override));

    MOCK_METHOD((std::unordered_map<std::string, Matrixify::InputTable>),
                readInputDir, (std::string const &), (override));

    MOCK_METHOD(Matrixify::Configuration, getConfiguration, (),
                (const, override));

    MOCK_METHOD(Matrixify::InputTable, loadTable, (std::string const &filename),
                (override));

    MOCK_METHOD(void, populateCostParameters, (), (override));

    MOCK_METHOD((std::vector<std::string>), getCostPerspectives, (),
                (const, override));

    MOCK_METHOD(bool, getCostSwitch, (), (const, override));
};

#endif