////////////////////////////////////////////////////////////////////////////////
// File: Writer.hpp                                                           //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

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

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

#include <respond/data_ops/data_loader.hpp>

/// @brief Namespace Defining all data_ops Operations
namespace data_ops {

    /// @brief Writer Class that acts as a superclass to all writing classes
    class Writer : public virtual IWritable, public virtual IFileable {
    public:
        /// @brief Primary Constructor for Writer class
        /// @param dirname string containing path to folder. Default is empty
        /// string
        /// @param timesteps Vector containing ints of time to record.
        /// @param writeType WriteType Enum describing the type of writing being
        /// done by the class
        Writer(std::string dirname = "", std::vector<int> timesteps = {},
               WriteType writeType = WriteType::STRING) {
            setDirname(dirname);
            this->timesteps = timesteps;
            setWriteType(writeType);
        }

        /// @brief
        /// @param opt
        void setWriteType(WriteType opt) override { this->writeType = opt; }

        /// @brief
        /// @return
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
        /// @return
        std::vector<int> getTimesteps() const { return timesteps; }

    protected:
        bool checkDirectory() const;
        std::string writeFile(const std::string filename,
                              std::stringstream &stream) const;

    private:
        std::string dirname;
        std::vector<int> timesteps;
        WriteType writeType;
    };

    class InputWriter : public Writer {
    public:
        InputWriter() : Writer("", {}, WriteType::STRING) {};
        InputWriter(std::string dirname, std::vector<int> timesteps,
                    WriteType writeType)
            : Writer(dirname, timesteps, writeType) {};

        std::string writeParameters(const std::shared_ptr<IDataLoader>) const;

    private:
        std::string
        writeOUDTransitionRates(const std::shared_ptr<IDataLoader>) const;

        std::string
        writeInterventionInitRates(const std::shared_ptr<IDataLoader>) const;

        std::string writeInterventionTransitionRates(
            const std::shared_ptr<IDataLoader>) const;

        std::string
        writeOverdoseRates(const std::shared_ptr<IDataLoader>) const;

        std::string
        writeFatalOverdoseRates(const std::shared_ptr<IDataLoader>) const;
    };

    class OutputWriter : public Writer {
    public:
        OutputWriter() : Writer("", {}, WriteType::STRING) {};
        OutputWriter(std::string dirname,
                     std::vector<std::string> interventions,
                     std::vector<std::string> oudStates,
                     std::vector<std::string> demographics,
                     std::vector<std::string> demographicCombos,
                     std::vector<int> timesteps, WriteType writeType,
                     bool pivot = false)
            : Writer(dirname, timesteps, writeType) {
            this->interventions = interventions;
            this->oudStates = oudStates;
            this->demographics = demographics;
            this->demographicCombos = demographicCombos;
            this->pivot = pivot;
        }

        std::vector<std::string> getInterventions() const {
            return this->interventions;
        }
        std::vector<std::string> getOUDStates() const {
            return this->oudStates;
        }
        std::vector<std::string> getDemographics() const {
            return this->demographics;
        }
        std::vector<std::string> getDemographicCombos() const {
            return this->demographicCombos;
        }
        bool getPivot() const { return this->pivot; }

        void setInterventions(std::vector<std::string> const v) {
            this->interventions = v;
        }
        void setOUDStates(std::vector<std::string> const v) {
            this->oudStates = v;
        }
        void setDemographics(std::vector<std::string> const v) {
            this->demographics = v;
        }
        void setDemographicCombos(std::vector<std::string> const v) {
            this->demographicCombos = v;
        }
        void setPivot(bool const p) { this->pivot = p; }

    protected:
        /// @brief
        /// @param timesteps
        /// @return
        std::string writeColumnHeaders() const;

        /// @brief
        /// @param stream
        /// @param historyToWrite
        void write4d(std::stringstream &stream, const Matrix4d historyToWrite,
                     const std::string columnHeaders) const;

    private:
        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        std::vector<std::string> demographics;
        std::vector<std::string> demographicCombos;
        bool pivot = false;
    };

    class HistoryWriter : public OutputWriter {
    public:
        HistoryWriter()
            : OutputWriter("", {}, {}, {}, {}, {}, WriteType::STRING) {};
        HistoryWriter(std::string dirname,
                      std::vector<std::string> interventions,
                      std::vector<std::string> oudStates,
                      std::vector<std::string> demographics,
                      std::vector<std::string> demographicCombos,
                      std::vector<int> timesteps, WriteType writeType,
                      bool pivot = false)
            : OutputWriter(dirname, interventions, oudStates, demographics,
                           demographicCombos, timesteps, writeType, pivot) {}

        /// @brief Main function to write the history
        /// @param writeType Enum describing the output type. Defaults to FILE
        /// @param history History struct containing the results of the
        /// simulation
        /// @return String depending on the output type
        std::string writeHistory(const History history) const;
    };

    class CostWriter : public OutputWriter {
    public:
        CostWriter()
            : OutputWriter("", {}, {}, {}, {}, {}, WriteType::STRING) {};
        CostWriter(std::string dirname, std::vector<std::string> interventions,
                   std::vector<std::string> oudStates,
                   std::vector<std::string> demographics,
                   std::vector<std::string> demographicCombos,
                   std::vector<int> timesteps, WriteType writeType,
                   bool pivot = false)
            : OutputWriter(dirname, interventions, oudStates, demographics,
                           demographicCombos, timesteps, writeType, pivot) {}
        /// @brief Main function to write the cost
        /// @param writeType Enum describing the output type. Defaults to FILE
        /// @param cost Cost struct containing the results of the simulation
        /// @return String depending on the output type
        virtual std::string writeCosts(const CostList costs) const;
    };

    class UtilityWriter : public OutputWriter {
    public:
        UtilityWriter()
            : OutputWriter("", {}, {}, {}, {}, {}, WriteType::STRING) {};
        UtilityWriter(std::string dirname,
                      std::vector<std::string> interventions,
                      std::vector<std::string> oudStates,
                      std::vector<std::string> demographics,
                      std::vector<std::string> demographicCombos,
                      std::vector<int> timesteps, WriteType writeType,
                      bool pivot = false)
            : OutputWriter(dirname, interventions, oudStates, demographics,
                           demographicCombos, timesteps, writeType, pivot) {}
        /// @brief Main function to write the Utilities
        /// @param writeType Enum describing the output type. Defaults to FILE
        /// @param util Utility struct containing the results of the simulation
        /// @return String depending on the output type
        std::string writeUtilities(const Matrix4d utilities) const;
    };

    class TotalsWriter : public OutputWriter {
    public:
        TotalsWriter()
            : OutputWriter("", {}, {}, {}, {}, {}, WriteType::STRING) {};
        TotalsWriter(std::string dirname,
                     std::vector<std::string> interventions,
                     std::vector<std::string> oudStates,
                     std::vector<std::string> demographics,
                     std::vector<std::string> demographicCombos,
                     std::vector<int> timesteps, WriteType writeType)
            : OutputWriter(dirname, interventions, oudStates, demographics,
                           demographicCombos, timesteps, writeType) {}
        std::string writeTotals(const Totals totals) const;
    };
} // namespace data_ops

#endif