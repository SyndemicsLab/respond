//===-- BaseLoader.cpp - BaseLoader class definition --------------------*- C++
//-*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the BaseLoader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "BaseLoader.hpp"

namespace Matrixify {
    BaseLoader::BaseLoader(std::string const &inputDir,
                           std::shared_ptr<spdlog::logger> logger) {
        if (!logger) {
            if (!spdlog::get("console")) {
                logger = spdlog::stdout_color_mt("console");
            } else {
                logger = spdlog::get("console");
            }
        }
        this->logger = logger;

        if (!inputDir.empty()) {
            std::filesystem::path inputPath = inputDir;
            inputPath = inputPath / "sim.conf";
            this->Config =
                std::make_shared<Data::Configuration>(inputPath.string());
            loadObjectData();
        }
    }

    bool BaseLoader::loadConfigurationFile(std::string const &configPath) {
        if (!configPath.empty()) {
            this->Config = std::make_shared<Data::Configuration>(configPath);
        }
        loadObjectData();
        return true;
    }

    bool
    BaseLoader::loadConfigurationPointer(Data::IConfigurationPtr configPtr) {
        this->Config = configPtr;
        loadObjectData();
        return true;
    }

    Data::IDataTablePtr BaseLoader::readCSV(std::string const &inputFile) {
        Data::IDataTablePtr table =
            std::make_shared<Data::DataTable>(inputFile);
        return table;
    }

    std::unordered_map<std::string, Data::IDataTablePtr>
    BaseLoader::readInputDir(std::string const &inputDir) {
        std::filesystem::path inputDirFixed = inputDir;
        std::unordered_map<std::string, Data::IDataTablePtr> toReturn;

        for (std::string inputFile : Matrixify::BaseLoader::INPUT_FILES) {
            std::filesystem::path filePath = inputDirFixed / inputFile;
            if (!std::filesystem::exists(filePath)) {
                // this->logger->warn("File " + filePath.string() +
                //                    " does not exist!");
                continue;
            }
            toReturn[inputFile] = readCSV(filePath.string());
        }
        return toReturn;
    }

    void BaseLoader::recursiveHelper(
        std::vector<std::vector<std::string>> &finalResultVector,
        std::vector<std::string> &currentResultVector,
        std::vector<std::vector<std::string>>::const_iterator currentInput,
        std::vector<std::vector<std::string>>::const_iterator finalInput) {
        if (currentInput == finalInput) {
            finalResultVector.push_back(currentResultVector);
            return;
        }
        const std::vector<std::string> &temp = *currentInput;
        for (std::vector<std::string>::const_iterator it = temp.begin();
             it != temp.end(); it++) {
            currentResultVector.push_back(*it);
            recursiveHelper(finalResultVector, currentResultVector,
                            currentInput + 1, finalInput);
            currentResultVector.pop_back();
        }
    }

    void BaseLoader::loadFromConfig() {
        if (!this->Config) {
            return; // do we want to throw an error or warn the user?
        }
        std::shared_ptr<Data::Configuration> derivedConfig =
            std::dynamic_pointer_cast<Data::Configuration>(this->Config);

        // simulation
        this->duration = derivedConfig->get<int>("simulation.duration");
        this->agingInterval =
            derivedConfig->get<int>("simulation.aging_interval");
        this->interventionChangeTimes =
            this->Config->getIntVector("simulation.intervention_change_times");
        this->enteringSampleChangeTimes = this->Config->getIntVector(
            "simulation.entering_sample_change_times");
        this->overdoseChangeTimes =
            this->Config->getIntVector("simulation.overdose_change_times");

        // state
        this->oudStates = this->Config->getStringVector("state.ouds");
        this->interventions =
            this->Config->getStringVector("state.interventions");

        // demographic
        // ageGroupShift Calculation. Assumes ages are reported as XX_XX
        std::vector<std::string> ages =
            this->Config->getStringVector("demographic.age_groups");
        std::string firstAgeGroup = ages[0];
        std::string ageDelimiter = "_";
        std::string firstAgeGroupStr =
            firstAgeGroup.substr(0, firstAgeGroup.find(ageDelimiter));
        int firstAge = std::stoi(firstAgeGroupStr);
        firstAgeGroup.erase(0,
                            firstAgeGroupStr.length() + ageDelimiter.length());
        int secondAge = std::stoi(firstAgeGroup);

        // have to add 1 for inclusive first value
        this->ageGroupShift = (secondAge - firstAge) + 1;
        this->demographics = derivedConfig->getSectionCategories("demographic");
        std::vector<std::vector<std::string>> inputDemographicVals;
        for (std::string demographic : this->demographics) {
            inputDemographicVals.push_back(
                this->Config->getStringVector("demographic." + demographic));
        }
        std::vector<std::vector<std::string>> temp1;
        std::vector<std::string> temp2;
        recursiveHelper(temp1, temp2, inputDemographicVals.begin(),
                        inputDemographicVals.end());
        for (std::vector<std::string> strList : temp1) {
            std::string group = "";
            for (std::string st : strList) {
                std::transform(st.begin(), st.end(), st.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                group += " " + st;
            }
            this->demographicCombos.push_back(group);
        }

        // cost
        this->costSwitch = derivedConfig->get<bool>("cost.cost_analysis");
        if (this->costSwitch) {
            this->costPerspectives =
                this->Config->getStringVector("cost.cost_perspectives");
            this->discountRate =
                derivedConfig->get<double>("cost.discount_rate");
            this->reportingInterval =
                derivedConfig->get<int>("cost.reporting_interval");
            this->costCategoryOutputs =
                derivedConfig->get<bool>("cost.cost_category_outputs");
            this->costUtilityOutputTimesteps = this->Config->getIntVector(
                "cost.cost_utility_output_timesteps");
        }

        // output
        this->perInterventionPredictions =
            derivedConfig->get<bool>("output.per_intervention_predictions");
        this->generalOutputsSwitch =
            derivedConfig->get<bool>("output.general_outputs");
        this->generalStatsOutputTimesteps =
            this->Config->getIntVector("output.general_stats_output_timesteps");
    }

    std::map<std::string, int>
    BaseLoader::buildIndiceMaps(std::vector<std::string> keys) const {
        std::map<std::string, int> idxMap = {};
        if (!keys.empty()) {
            for (int i = 0; i < keys.size(); ++i) {
                idxMap[keys[i]] = i;
            }
        }
        return idxMap;
    }

    void BaseLoader::loadObjectData() {
        loadFromConfig();
        this->interventionsIndices = buildIndiceMaps(getInterventions());
        this->oudIndices = buildIndiceMaps(getOUDStates());
        this->demographicComboIndices = buildIndiceMaps(getDemographicCombos());
    }

    // tabular files from the current RESPOND directory structure, as of
    // [2023-04-06]
    const std::vector<std::string> BaseLoader::INPUT_FILES = {
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

} // namespace Matrixify
