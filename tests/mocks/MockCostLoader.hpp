#ifndef TESTS_MOCKCOSTLOADER_HPP_
#define TESTS_MOCKCOSTLOADER_HPP_

#include "CostLoader.hpp"
#include "gmock/gmock.h"

class MockCostLoader : public Data::ICostLoader {
public:
    MOCK_METHOD((std::unordered_map<std::string, Data::Matrix3d>),
                loadHealthcareUtilizationCost, (std::string const &csvName),
                (override));

    MOCK_METHOD((std::unordered_map<std::string,
                                    std::unordered_map<std::string, double>>),
                loadOverdoseCost, (std::string const &csvName), (override));

    MOCK_METHOD((std::unordered_map<std::string, Data::Matrix3d>),
                loadPharmaceuticalCost, (std::string const &csvName),
                (override));

    MOCK_METHOD((std::unordered_map<std::string, Data::Matrix3d>),
                loadTreatmentUtilizationCost, (std::string const &csvName),
                (override));

    MOCK_METHOD(void, populateCostParameters, (), (override));

    MOCK_METHOD(Data::Matrix3d, getHealthcareUtilizationCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(Data::Matrix3d, getPharmaceuticalCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(Data::Matrix3d, getTreatmentUtilizationCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(double, getNonFatalOverdoseCost,
                (std::string const &perspective), (const, override));

    MOCK_METHOD(double, getFatalOverdoseCost, (std::string const &perspective),
                (const, override));

    MOCK_METHOD((std::vector<std::string>), getCostPerspectives, (),
                (const, override));

    MOCK_METHOD(bool, getCostSwitch, (), (const, override));

    MOCK_METHOD(double, getDiscountRate, (), (const, override));

    MOCK_METHOD(Data::Configuration, loadConfigurationFile,
                (std::string const &configPath), (override));

    /// @brief Reads a configuration file to a Configuration object
    /// @param inputFile path to the configuration file to be read
    /// @return
    MOCK_METHOD(Data::Configuration, readConfigFile, (std::string const &),
                (override));
    /// @brief Read a CSV-formatted file into a map object where the headers
    /// are keys and the rest of the columns are stored as vectors of
    /// strings
    /// @param inputFile path to the CSV to be read
    /// @return A map object containing columns stored as key-value pairs
    MOCK_METHOD(Data::InputTable, readCSV, (std::string const &), (override));

    /// @brief Reads typical RESPOND input files from the provided input
    /// directory
    /// @param inputDir the directory from which to read input files
    /// @return an unordered map whose keys are table names and values are
    /// CSV/InputTables
    MOCK_METHOD((std::unordered_map<std::string, Data::InputTable>),
                readInputDir, (std::string const &), (override));

    /// @brief Get the Configuration from the Loader
    /// @return Configuration
    MOCK_METHOD(Data::Configuration, getConfiguration, (), (const, override));

    MOCK_METHOD(Data::InputTable, loadTable, (std::string const &filename),
                (override));
};

#endif