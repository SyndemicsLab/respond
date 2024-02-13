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

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <DataManagement.hpp>

namespace Matrixify {
    class ILoader {
    public:
        virtual bool loadConfigurationFile(std::string const &configPath) = 0;

        virtual bool
        loadConfigurationPointer(Data::IConfigurationPtr configPtr) = 0;

        /// @brief Read a CSV-formatted file into a map object where the headers
        /// are keys and the rest of the columns are stored as vectors of
        /// strings
        /// @param inputFile path to the CSV to be read
        /// @return A map object containing columns stored as key-value pairs
        virtual Data::IDataTablePtr readCSV(std::string const &) = 0;

        /// @brief Reads typical RESPOND input files from the provided input
        /// directory
        /// @param inputDir the directory from which to read input files
        /// @return an unordered map whose keys are table names and values are
        /// CSV/Data::IDataTablePtrs
        virtual std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &) = 0;

        /// @brief Get the Configuration from the Loader
        /// @return Configuration
        virtual Data::IConfigurationPtr getConfiguration() const = 0;

        virtual Data::IDataTablePtr loadTable(std::string const &filename) = 0;

        // simulation
        virtual int getDuration() const = 0;
        virtual int getAgingInterval() const = 0;
        virtual std::vector<int> getInterventionChangeTimes() const = 0;
        virtual std::vector<int> getEnteringSampleChangeTimes() const = 0;
        virtual std::vector<int> getOverdoseChangeTimes() const = 0;

        // state
        virtual std::vector<std::string> getInterventions() const = 0;
        virtual std::vector<std::string> getOUDStates() const = 0;
        virtual int getNumOUDStates() const = 0;
        virtual int getNumInterventions() const = 0;

        // demographic
        virtual std::vector<std::string> getDemographics() const = 0;
        virtual int getNumDemographics() const = 0;
        virtual std::vector<std::string> getDemographicCombos() const = 0;
        virtual int getNumDemographicCombos() const = 0;
        virtual int getAgeGroupShift() const = 0;

        // cost
        virtual bool getCostSwitch() const = 0;
        virtual std::vector<std::string> getCostPerspectives() const = 0;
        virtual double getDiscountRate() const = 0;
        virtual bool getCostCategoryOutputs() const = 0;
        virtual std::vector<int> getCostUtilityOutputTimesteps() const = 0;

        // output
        virtual bool getPerInterventionPredictions() const = 0;
        virtual bool getGeneralOutputsSwitch() const = 0;
        virtual std::vector<int> getGeneralStatsOutputTimesteps() const = 0;
    };

    class Loader : public virtual ILoader {
    public:
        static const std::vector<std::string> INPUT_FILES;
        Loader() : Loader("", nullptr){};
        Loader(std::string const &inputDir);
        Loader(std::shared_ptr<spdlog::logger> logger);
        Loader(std::string const &inputDir,
               std::shared_ptr<spdlog::logger> logger);

        bool loadConfigurationFile(std::string const &configPath);

        bool loadConfigurationPointer(Data::IConfigurationPtr configPtr);

        virtual Data::IDataTablePtr readCSV(std::string const &);

        virtual std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &);

        virtual Data::IConfigurationPtr getConfiguration() const {
            return Config;
        }

        virtual Data::IDataTablePtr loadTable(std::string const &filename) {
            if (this->inputTables.find(filename) == this->inputTables.end()) {
                this->inputTables[filename] = Loader::readCSV(filename);
            }
            return this->inputTables[filename];
        }

        // simulation
        virtual int getDuration() const { return duration; }
        virtual int getAgingInterval() const { return agingInterval; }
        virtual std::vector<int> getInterventionChangeTimes() const {
            return interventionChangeTimes;
        }
        virtual std::vector<int> getEnteringSampleChangeTimes() const {
            return enteringSampleChangeTimes;
        }
        virtual std::vector<int> getOverdoseChangeTimes() const {
            return overdoseChangeTimes;
        }

        // state
        virtual std::vector<std::string> getInterventions() const {
            return interventions;
        }
        virtual std::vector<std::string> getOUDStates() const {
            return oudStates;
        }
        virtual int getNumOUDStates() const { return this->oudStates.size(); }
        virtual int getNumInterventions() const {
            return this->interventions.size();
        }

        // demographic
        virtual std::vector<std::string> getDemographics() const {
            return this->demographics;
        }
        virtual int getNumDemographics() const {
            return this->demographics.size();
        }
        virtual std::vector<std::string> getDemographicCombos() const {
            return this->demographicCombos;
        }
        virtual int getNumDemographicCombos() const {
            return this->demographicCombos.size();
        }
        virtual int getAgeGroupShift() const { return ageGroupShift; }

        // cost
        virtual bool getCostSwitch() const { return costSwitch; }
        virtual std::vector<std::string> getCostPerspectives() const {
            return costPerspectives;
        }
        virtual double getDiscountRate() const { return discountRate; }
        virtual bool getCostCategoryOutputs() const {
            return costCategoryOutputs;
        }
        virtual std::vector<int> getCostUtilityOutputTimesteps() const {
            return costUtilityOutputTimesteps;
        }

        // output
        virtual bool getPerInterventionPredictions() const {
            return perInterventionPredictions;
        }
        virtual bool getGeneralOutputsSwitch() const {
            return generalOutputsSwitch;
        }
        virtual std::vector<int> getGeneralStatsOutputTimesteps() const {
            return generalStatsOutputTimesteps;
        }

    protected:
        std::unordered_map<std::string, Data::IDataTablePtr> inputTables;
        std::shared_ptr<spdlog::logger> logger;

        // simulation
        int duration;
        int agingInterval;
        std::vector<int> interventionChangeTimes;
        std::vector<int> enteringSampleChangeTimes;
        std::vector<int> overdoseChangeTimes;

        // state
        std::vector<std::string> interventions;
        std::map<std::string, int> interventionsIndices;
        std::vector<std::string> oudStates;
        std::map<std::string, int> oudIndices;

        // demographic
        int ageGroupShift;
        std::vector<std::string> demographics;
        std::vector<std::string> demographicCombos = {};
        std::map<std::string, int> demographicComboIndices;

        // cost
        bool costSwitch = false;
        std::vector<std::string> costPerspectives = {};
        double discountRate = 0.0;
        int reportingInterval = 1;
        bool costCategoryOutputs = false;
        std::vector<int> costUtilityOutputTimesteps = {};

        // output
        bool perInterventionPredictions;
        bool generalOutputsSwitch;
        std::vector<int> generalStatsOutputTimesteps;

        void loadObjectData();

    private:
        Data::IConfigurationPtr Config;
        void loadFromConfig();
        std::map<std::string, int>
        buildIndiceMaps(std::vector<std::string> keys) const;
        void recursiveHelper(
            std::vector<std::vector<std::string>> &finalResultVector,
            std::vector<std::string> &currentResultVector,
            std::vector<std::vector<std::string>>::const_iterator currentInput,
            std::vector<std::vector<std::string>>::const_iterator finalInput);
    };
} // namespace Matrixify
#endif
