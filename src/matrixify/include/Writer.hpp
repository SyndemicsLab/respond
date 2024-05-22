//===-- Writer.hpp - Writer class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Writer class.
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
#include <variant>
#include <vector>

#include "DataLoader.hpp"
#include "Interfaces.hpp"

/// @brief Namespace Defining all Matrixify Operations
namespace Matrixify {

    class Writer : public virtual IWritable,
                   public virtual IFileable,
                   public virtual IStratifiable {
    public:
        Writer(std::string dirname = "",
               std::vector<std::string> interventions = {},
               std::vector<std::string> oudStates = {},
               std::vector<std::string> demographics = {},
               std::vector<std::string> demographicCombos = {},
               std::vector<int> timesteps = {},
               WriteType writeType = WriteType::STRING) {
            setDirname(dirname);
            setInterventions(interventions);
            setOUDStates(oudStates);
            setDemographics(demographics);
            setDemographicCombos(demographicCombos);
            this->timesteps = timesteps;
            setWriteType(writeType);
        }

        void setWriteType(WriteType opt) override { this->writeType = opt; }

        WriteType getWriteType() const override { return this->writeType; }

        /// @brief
        /// @param dirname
        void setDirname(std::string dirname) override {
            this->dirname = dirname;
        }

        /// @brief
        /// @return
        std::string getDirname() const override { return dirname; }

        /// @brief
        /// @param interventions
        void setInterventions(std::vector<std::string> interventions) override {
            this->interventions = interventions;
        }

        /// @brief
        /// @param oudStates
        void setOUDStates(std::vector<std::string> oudStates) override {
            this->oudStates = oudStates;
        }

        /// @brief
        /// @param demographics
        void setDemographics(std::vector<std::string> demographics) override {
            this->demographics = demographics;
        }

        void setDemographicCombos(
            std::vector<std::string> demographicCombos) override {
            this->demographicCombos = demographicCombos;
        }

        std::vector<std::string> getInterventions() const override {
            return this->interventions;
        }
        std::vector<std::string> getOUDStates() const override {
            return this->oudStates;
        }
        std::vector<std::string> getDemographics() const override {
            return this->demographics;
        }
        std::vector<std::string> getDemographicCombos() const override {
            return this->demographicCombos;
        }

    protected:
        /// @brief
        /// @param timesteps
        /// @return
        std::string writeColumnHeaders() const;

        /// @brief
        /// @param stream
        /// @param historyToWrite
        void writer(std::ostream &stream, const Matrix4d historyToWrite) const;

    private:
        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        std::vector<std::string> demographics;
        std::vector<std::string> demographicCombos;
        std::string dirname;
        std::vector<int> timesteps;
        WriteType writeType;
    };

    class InputWriter : public virtual Writer {
    public:
        InputWriter() : Writer("", {}, {}, {}, {}, {}, WriteType::STRING){};
        InputWriter(std::string dirname, std::vector<std::string> interventions,
                    std::vector<std::string> oudStates,
                    std::vector<std::string> demographics,
                    std::vector<std::string> demographicCombos,
                    std::vector<int> timesteps, WriteType writeType)
            : Writer(dirname, interventions, oudStates, demographics,
                     demographicCombos, timesteps, writeType){};

        std::string writeInputs(const DataLoader) const;

    private:
        std::string writeOUDTransitionRates(Matrix3d) const;

        std::string writeInterventionInitRates(Matrix3d) const;

        std::string writeInterventionTransitionRates(Matrix4d) const;

        std::string writeOverdoseRates(Matrix4d) const;

        std::string writeFatalOverdoseRates(Matrix4d) const;
    };

    class HistoryWriter : public virtual Writer {
    public:
        HistoryWriter() : Writer("", {}, {}, {}, {}, {}, WriteType::STRING){};
        HistoryWriter(std::string dirname,
                      std::vector<std::string> interventions,
                      std::vector<std::string> oudStates,
                      std::vector<std::string> demographics,
                      std::vector<std::string> demographicCombos,
                      std::vector<int> timesteps, WriteType writeType)
            : Writer(dirname, interventions, oudStates, demographics,
                     demographicCombos, timesteps, writeType){};
        /// @brief Main function to write the history
        /// @param writeType Enum describing the output type. Defaults to FILE
        /// @param history History struct containing the results of the
        /// simulation
        /// @return String depending on the output type
        std::string writeHistory(const History history) const;
    };

    class CostWriter : public virtual Writer {
    public:
        CostWriter() : Writer("", {}, {}, {}, {}, {}, WriteType::STRING){};
        CostWriter(std::string dirname, std::vector<std::string> interventions,
                   std::vector<std::string> oudStates,
                   std::vector<std::string> demographics,
                   std::vector<std::string> demographicCombos,
                   std::vector<int> timesteps, WriteType writeType)
            : Writer(dirname, interventions, oudStates, demographics,
                     demographicCombos, timesteps, writeType){};
        /// @brief Main function to write the cost
        /// @param writeType Enum describing the output type. Defaults to FILE
        /// @param cost Cost struct containing the results of the simulation
        /// @return String depending on the output type
        virtual std::string writeCosts(const CostList costs) const;
    };

    class UtilityWriter : public virtual Writer {
    public:
        UtilityWriter() : Writer("", {}, {}, {}, {}, {}, WriteType::STRING){};
        UtilityWriter(std::string dirname,
                      std::vector<std::string> interventions,
                      std::vector<std::string> oudStates,
                      std::vector<std::string> demographics,
                      std::vector<std::string> demographicCombos,
                      std::vector<int> timesteps, WriteType writeType)
            : Writer(dirname, interventions, oudStates, demographics,
                     demographicCombos, timesteps, writeType){};
        /// @brief Main function to write the Utilities
        /// @param writeType Enum describing the output type. Defaults to FILE
        /// @param util Utility struct containing the results of the simulation
        /// @return String depending on the output type
        std::string writeUtilities(const Matrix4d utilities) const;
    };

    class TotalsWriter : public virtual Writer {
    public:
        TotalsWriter() : Writer("", {}, {}, {}, {}, {}, WriteType::STRING){};
        TotalsWriter(std::string dirname,
                     std::vector<std::string> interventions,
                     std::vector<std::string> oudStates,
                     std::vector<std::string> demographics,
                     std::vector<std::string> demographicCombos,
                     std::vector<int> timesteps, WriteType writeType)
            : Writer(dirname, interventions, oudStates, demographics,
                     demographicCombos, timesteps, writeType){};
        std::string writeTotals(const Totals totals) const;
    };
} // namespace Matrixify

#endif