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

#include <Eigen/Eigen>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>
#include <vector>

#include <fmt/core.h>

#include "DataTypes.hpp"
#include "Simulation.hpp"

/// @brief Namespace Defining all Data Operations
namespace Data {

    /// @brief Type used to determine the output expected, used primarily in
    /// testing
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
        virtual void
        setInterventions(std::vector<std::string> interventions) = 0;

        /// @brief Specify the OUD States
        /// @param oudStates Vector of Strings of OUD States
        virtual void setOUDStates(std::vector<std::string> oudStates) = 0;

        /// @brief Specify the Demographics
        /// @param demographics Vector of Vector of Strings of Demographics
        virtual void
        setDemographics(std::vector<std::vector<std::string>> demographics) = 0;

        /// @brief Main function to write the history
        /// @param outputType Enum describing the output type. Defaults to FILE
        /// @param history History struct containing the results of the
        /// simulation
        /// @return String depending on the output type
        virtual std::string writeHistory(OutputType outputType,
                                         History history) = 0;

        /// @brief Main function to write the cost
        /// @param outputType Enum describing the output type. Defaults to FILE
        /// @param cost Cost struct containing the results of the simulation
        /// @return String depending on the output type
        virtual std::string writeCosts(OutputType outputType,
                                       CostList costs) = 0;

        /// @brief Main function to write the Utilities
        /// @param outputType Enum describing the output type. Defaults to FILE
        /// @param util Utility struct containing the results of the simulation
        /// @return String depending on the output type
        virtual std::string writeUtilities(OutputType outputType,
                                           Matrix3dOverTime utilities) = 0;
    };

    /// @brief Concrete DataWriter class implementing the corresponding
    /// interface
    class DataWriter : public IDataWriter {
    public:
        /// @brief
        DataWriter();

        /// @brief
        /// @param dirname
        /// @param interventions
        /// @param oudStates
        /// @param demographics
        DataWriter(std::string dirname, std::vector<std::string> interventions,
                   std::vector<std::string> oudStates,
                   std::vector<std::vector<std::string>> demographics,
                   std::vector<int> timesteps, bool writeState);

        /// @brief
        /// @param dirname
        void addDirname(std::string dirname) override;

        /// @brief
        /// @return
        std::string getDirname() override;

        /// @brief
        /// @param interventions
        void setInterventions(std::vector<std::string> interventions) override;

        /// @brief
        /// @param oudStates
        void setOUDStates(std::vector<std::string> oudStates) override;

        /// @brief
        /// @param demographics
        void setDemographics(
            std::vector<std::vector<std::string>> demographics) override;

        /// @brief
        /// @param outputType
        /// @param history
        /// @return
        std::string writeHistory(OutputType outputType,
                                 History history) override;

        /// @brief
        /// @param outputType
        /// @param cost
        /// @return
        std::string writeCosts(OutputType outputType, CostList costs) override;

        /// @brief
        /// @param outputType
        /// @param util
        /// @return
        std::string writeUtilities(OutputType outputType,
                                   Matrix3dOverTime utilities) override;

    private:
        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        std::vector<std::vector<std::string>> demographics;
        std::string dirname;
        std::vector<int> timesteps;
        bool writeState;

        /// @brief
        /// @param timesteps
        /// @return
        std::string writeColumnHeaders();

        /// @brief
        /// @param stream
        /// @param historyToWrite
        void writer(std::ostream &stream, Matrix3dOverTime historyToWrite);
    };
} // namespace Data

#endif