////////////////////////////////////////////////////////////////////////////////
// File: BaseLoader.cpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/BaseLoader.hpp>

namespace data_ops {
    BaseLoader::BaseLoader(Data::IConfigablePtr config,
                           std::string const &inputDir,
                           std::shared_ptr<spdlog::logger> logger) {

        if (config) {
            loadConfigPtr(config);
        }

        if (!inputDir.empty()) {
            std::filesystem::path inputPath = inputDir;
            this->inputTables = this->readInputDir(inputDir);
            if (!this->Config) {
                if (inputPath.filename().string().compare("sim.conf") != 0) {
                    inputPath = inputPath / "sim.conf";
                }
                if (!loadConfigFile(inputPath.string())) {
                    this->logger->error("Config File Not Found! Exiting");
                    exit(-1);
                    // error on config file being found
                }
            }
        }

        setLogger(logger);
    }

    bool BaseLoader::loadConfigFile(std::string const &configPath) {
        if (configPath.empty() || !std::filesystem::exists(configPath)) {
            return false;
        }
        this->Config = std::make_shared<Data::Config>(configPath);
        loadObjectData();
        return true;
    }

    bool BaseLoader::loadConfigPtr(Data::IConfigablePtr configPtr) {
        if (!configPtr) {
            return false;
        }
        this->Config = configPtr;
        loadObjectData();
        return true;
    }

    Data::IDataTablePtr BaseLoader::readCSV(std::string const &inputFile,
                                            bool headers) {
        Data::IDataTablePtr table =
            std::make_shared<Data::DataTable>(inputFile, headers, ',');
        return table;
    }

    std::unordered_map<std::string, Data::IDataTablePtr>
    BaseLoader::readInputDir(std::string const &inputDir) {
        std::filesystem::path inputDirFixed = inputDir;
        std::unordered_map<std::string, Data::IDataTablePtr> toReturn;

        for (std::string inputFile : data_ops::BaseLoader::INPUT_FILES) {
            std::filesystem::path filePath = inputDirFixed / inputFile;
            if (!std::filesystem::exists(filePath)) {

                // TODO This causes an error during tests
                // #ifdef NDEBUG
                //                 this->logger->warn("File " +
                //                 filePath.string() +
                //                                    " does not exist!");
                // #endif
                continue;
            }
            toReturn[inputFile] = readCSV(filePath.string(), true);
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
#ifndef NDEBUG
            this->logger->warn(
                "Config not found when trying to load from Config File.");
#endif
            return;
        }
        Data::IConfigablePtr derivedConfig =
            std::dynamic_pointer_cast<Data::Config>(this->Config);

        // simulation
        this->duration =
            std::get<int>(derivedConfig->get("simulation.duration", (int)0));
        this->enterCohortToggle = std::get<bool>(
            derivedConfig->get("simulation.stratified_entering_cohort", false));
        this->agingInterval = std::get<int>(
            derivedConfig->get("simulation.aging_interval", (int)0));

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
        this->costSwitch =
            std::get<bool>(derivedConfig->get("cost.cost_analysis", false));
        if (this->costSwitch) {
            this->costPerspectives =
                this->Config->getStringVector("cost.cost_perspectives");
            this->discountRate =
                std::get<double>(derivedConfig->get("cost.discount_rate", 0.0));
            this->reportingInterval =
                std::get<int>(derivedConfig->get("cost.reporting_interval", 1));
            this->costCategoryOutputs = std::get<bool>(
                derivedConfig->get("cost.cost_category_outputs", false));
            this->costUtilityOutputTimesteps = this->Config->getIntVector(
                "cost.cost_utility_output_timesteps");
        }

        // output
        this->perInterventionPredictions = std::get<bool>(
            derivedConfig->get("output.per_intervention_predictions", false));
        this->generalOutputsSwitch =
            std::get<bool>(derivedConfig->get("output.general_outputs", false));
        this->generalStatsOutputTimesteps =
            this->Config->getIntVector("output.general_stats_output_timesteps");
    }

    std::vector<std::string> BaseLoader::getAgeGroupBins() const {
        if (!this->Config) {
            logger->error("No Configuration File Found!");
            return {};
        }
        return this->Config->getStringVector("demographic.age_groups");
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

} // namespace data_ops
