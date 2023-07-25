//===-- DataWriter.hpp - DataWriter class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the DataWriter class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef DATA_DATAWRITER_HPP_
#define DATA_DATAWRITER_HPP_

#define FMT_HEADER_ONLY

#include <eigen3/Eigen/Eigen>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <vector>

#include <fmt/core.h>

#include "DataTypes.hpp"
#include "Simulation.hpp"

/// @brief Namespace Defining all Data Operations
namespace Data {

/// @brief Type used to determine the output expected, used primarily in testing
enum OutputType { FILE = 0, STRING = 1 };

/// @brief Interface for DataWriter Object
class IDataWriter {
public:
    virtual ~IDataWriter(){};

    /// @brief Specify the Directory Name
    /// @param dirname Directory location to write files
    virtual void addDirname(std::string dirname) = 0;

    /// @brief Get the Directory Name
    /// @return String specifying directory
    virtual std::string getDirname() = 0;

    /// @brief Specify the Interventions
    /// @param interventions Vector of Strings of Interventions
    virtual void setInterventions(std::vector<std::string> interventions) = 0;

    /// @brief Specify the OUD States
    /// @param oudStates Vector of Strings of OUD States
    virtual void setOUDStates(std::vector<std::string> oudStates) = 0;

    /// @brief Specify the Demographics
    /// @param demographics Vector of Vector of Strings of Demographics
    virtual void
    setDemographics(std::vector<std::vector<std::string>> demographics) = 0;

    /// @brief Main function to write the history
    /// @param outputType Enum describing the output type. Defaults to FILE
    /// @param history History struct containing the results of the simulation
    /// @return String depending on the output type
    virtual std::string writeHistory(OutputType outputType,
                                     History history) = 0;

    /// @brief Main function to write the cost
    /// @param outputType Enum describing the output type. Defaults to FILE
    /// @param cost Cost struct containing the results of the simulation
    /// @return String depending on the output type
    virtual std::string writeCost(OutputType outputType, Cost cost) = 0;

    /// @brief Main function to write the Utilities
    /// @param outputType Enum describing the output type. Defaults to FILE
    /// @param util Utility struct containing the results of the simulation
    /// @return String depending on the output type
    virtual std::string writeUtility(OutputType outputType, Utility util) = 0;
};

/// @brief Concrete DataWriter class implementing the corresponding interface
class DataWriter : public IDataWriter {
public:
    DataWriter();
    DataWriter(std::string dirname, std::vector<std::string> interventions,
               std::vector<std::string> oudStates,
               std::vector<std::vector<std::string>> demographics);
    void addDirname(std::string dirname) override;
    std::string getDirname() override;
    void setInterventions(std::vector<std::string> interventions) override;
    void setOUDStates(std::vector<std::string> oudStates) override;
    void setDemographics(
        std::vector<std::vector<std::string>> demographics) override;
    std::string writeHistory(OutputType outputType, History history) override;
    std::string writeCost(OutputType outputType, Cost cost) override;
    std::string writeUtility(OutputType outputType, Utility util) override;

private:
    std::vector<std::string> interventions;
    std::vector<std::string> oudStates;
    std::vector<std::vector<std::string>> demographics;
    std::string dirname;
    std::string writeColumnHeaders(int timesteps);
    void writer(std::ostream &stream, Matrix3dOverTime historyToWrite);
};
} // namespace Data

#endif