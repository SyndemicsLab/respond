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

#include "InterfaceLoaders.hpp"

namespace Matrixify {
    class BaseLoader : public virtual IBaseLoader {
    public:
        BaseLoader(Data::IConfigablePtr config = nullptr,
                   std::string const &inputDir = "",
                   std::shared_ptr<spdlog::logger> logger = nullptr);

        bool loadConfigFile(std::string const &configPath) override;

        bool loadConfigPtr(Data::IConfigablePtr configPtr) override;

        Data::IDataTablePtr readCSV(std::string const &) override;

        std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &) override;

        Data::IConfigablePtr getConfig() const override { return Config; }

        Data::IDataTablePtr loadTable(std::string const &filename) override {
            if (this->inputTables.find(filename) == this->inputTables.end()) {
                this->inputTables[filename] = BaseLoader::readCSV(filename);
            }
            return this->inputTables[filename];
        }

        // simulation
        int getDuration() const override { return duration; }
        int getAgingInterval() const override { return agingInterval; }
        std::vector<int> getInterventionChangeTimes() const override {
            return interventionChangeTimes;
        }
        std::vector<int> getEnteringSampleChangeTimes() const override {
            return enteringSampleChangeTimes;
        }
        std::vector<int> getOverdoseChangeTimes() const override {
            return overdoseChangeTimes;
        }

        // state
        std::vector<std::string> getInterventions() const override {
            return interventions;
        }
        std::vector<std::string> getOUDStates() const override {
            return oudStates;
        }
        int getNumOUDStates() const override { return this->oudStates.size(); }
        int getNumInterventions() const override {
            return this->interventions.size();
        }

        std::vector<std::string> getAgeGroupBins() const override;

        // demographic
        std::vector<std::string> getDemographics() const override {
            return this->demographics;
        }
        int getNumDemographics() const override {
            return this->demographics.size();
        }
        std::vector<std::string> getDemographicCombos() const override {
            return this->demographicCombos;
        }
        int getNumDemographicCombos() const override {
            return this->demographicCombos.size();
        }
        int getAgeGroupShift() const override { return ageGroupShift; }

        // cost
        bool getCostSwitch() const override { return costSwitch; }
        std::vector<std::string> getCostPerspectives() const override {
            return costPerspectives;
        }
        double getDiscountRate() const override { return discountRate; }
        bool getCostCategoryOutputs() const override {
            return costCategoryOutputs;
        }
        std::vector<int> getCostUtilityOutputTimesteps() const override {
            return costUtilityOutputTimesteps;
        }

        // output
        bool getPerInterventionPredictions() const override {
            return perInterventionPredictions;
        }
        bool getGeneralOutputsSwitch() const override {
            return generalOutputsSwitch;
        }
        std::vector<int> getGeneralStatsOutputTimesteps() const override {
            return generalStatsOutputTimesteps;
        }

        std::shared_ptr<spdlog::logger> getLogger() const override {
            return logger;
        }

        void setLogger(std::shared_ptr<spdlog::logger> const log) {
            if (!log) {
                if (!spdlog::get("console")) {
                    this->logger = spdlog::stdout_color_mt("console");
                } else {
                    this->logger = spdlog::get("console");
                }
            } else {
                this->logger = log;
            }
        }

    protected:
        static const std::vector<std::string> INPUT_FILES;
        std::unordered_map<std::string, Data::IDataTablePtr> inputTables = {};
        Data::IConfigablePtr Config;
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
