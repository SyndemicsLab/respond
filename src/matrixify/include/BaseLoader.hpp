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
    class BaseLoader {
    public:
        BaseLoader(std::string const &inputDir = "",
                   std::shared_ptr<spdlog::logger> logger = nullptr);

        virtual bool loadConfigurationFile(std::string const &configPath);

        virtual bool
        loadConfigurationPointer(Data::IConfigurationPtr configPtr);

        virtual Data::IDataTablePtr readCSV(std::string const &);

        virtual std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &);

        virtual Data::IConfigurationPtr getConfiguration() const {
            return Config;
        }

        virtual Data::IDataTablePtr loadTable(std::string const &filename) {
            if (this->inputTables.find(filename) == this->inputTables.end()) {
                this->inputTables[filename] = BaseLoader::readCSV(filename);
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

        virtual std::shared_ptr<spdlog::logger> getLogger() const {
            return logger;
        }

    protected:
        static const std::vector<std::string> INPUT_FILES;
        std::unordered_map<std::string, Data::IDataTablePtr> inputTables = {};
        std::shared_ptr<spdlog::logger> logger;

        // simulation
        int duration = 0;
        int agingInterval = 0;
        std::vector<int> interventionChangeTimes = {};
        std::vector<int> enteringSampleChangeTimes = {};
        std::vector<int> overdoseChangeTimes = {};

        // state
        std::vector<std::string> interventions = {};
        std::map<std::string, int> interventionsIndices;
        std::vector<std::string> oudStates = {};
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
