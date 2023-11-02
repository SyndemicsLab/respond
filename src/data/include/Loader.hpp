//===-- Loader.hpp - Loader class definition --------------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Loader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef DATA_LOADER_HPP_
#define DATA_LOADER_HPP_

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/tokenizer.hpp>

#include "Configuration.hpp"

namespace Data {
    /// @brief The object in which values from CSV files are stored
    ///
    /// InputTable stores the tables as key, value pairs where the keys are
    /// column headers and the values are vectors containing entire columns.
    using InputTable =
        std::unordered_map<std::string, std::vector<std::string>>;

    // tabular files from the current RESPOND directory structure, as of
    // [2023-04-06]
    static std::vector<std::string> INPUT_FILES = {
        "all_types_overdose.csv",
        "background_mortality.csv",
        "block_init_effect.csv",
        "block_trans.csv",
        "entering_cohort.csv",
        "fatal_overdose.csv",
        "init_cohort.csv",
        "oud_trans.csv",
        "SMR.csv",
        "bg_utility.csv",
        "healthcare_utilization_cost.csv",
        "oud_utility.csv",
        "overdose_cost.csv",
        "pharmaceutical_cost.csv",
        "setting_utility.csv",
        "treatment_utilization_cost.csv"};

    class ILoader {
    public:
        virtual Configuration
        loadConfigurationFile(std::string const &configPath) = 0;

        /// @brief Reads a configuration file to a Configuration object
        /// @param inputFile path to the configuration file to be read
        /// @return
        virtual Configuration readConfigFile(std::string const &) = 0;
        /// @brief Read a CSV-formatted file into a map object where the headers
        /// are keys and the rest of the columns are stored as vectors of
        /// strings
        /// @param inputFile path to the CSV to be read
        /// @return A map object containing columns stored as key-value pairs
        virtual InputTable readCSV(std::string const &) = 0;

        /// @brief Reads typical RESPOND input files from the provided input
        /// directory
        /// @param inputDir the directory from which to read input files
        /// @return an unordered map whose keys are table names and values are
        /// CSV/InputTables
        virtual std::unordered_map<std::string, InputTable>
        readInputDir(std::string const &) = 0;

        /// @brief Get the Configuration from the Loader
        /// @return Configuration
        virtual Configuration getConfiguration() const = 0;

        virtual InputTable loadTable(std::string const &filename) = 0;
    };

    class Loader : public virtual ILoader {
    public:
        Loader(){};
        Loader(std::string const &inputDir);

        virtual Configuration
        loadConfigurationFile(std::string const &configPath);

        virtual Configuration readConfigFile(std::string const &);

        virtual InputTable readCSV(std::string const &);

        virtual std::unordered_map<std::string, InputTable>
        readInputDir(std::string const &);

        virtual Configuration getConfiguration() const { return this->Config; }

        virtual InputTable loadTable(std::string const &filename) {
            if (this->inputTables.find(filename) == this->inputTables.end()) {
                this->inputTables[filename] = Loader::readCSV(filename);
            }
            return this->inputTables[filename];
        }

    protected:
        std::unordered_map<std::string, InputTable> inputTables;
        Configuration Config;
    };

} // namespace Data
#endif
