////////////////////////////////////////////////////////////////////////////////
// File: base_loader_internals.hpp                                            //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_

#include <algorithm>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <datamanagement/DataManagement.hpp>

#include <respond/utils/logging.hpp>

namespace respond::data_ops {
    class BaseLoader {
    public:
        BaseLoader(const std::string &directory = "",
                   const std::string &log_name = "console")
            : logger_name(log_name) {

            if (directory.empty()) {
                std::stringstream message;
                message << "Invalid Directory provided to loader: " << directory
                        << std::endl
                        << "Fatal Error! Exiting...";
                respond::utils::LogError(log_name, message.str());
                exit(-1);
            }
            ReadInputDirectory(directory);
        }

        Data::IDataTablePtr ReadCSV(const std::string &path,
                                    bool headers = true) {
            Data::IDataTablePtr table =
                std::make_shared<Data::DataTable>(path, headers, ',');
            return table;
        }

        void ReadInputDirectory(const std::filesystem::path &directory) {
            std::filesystem::path conf = directory / "sim.conf";
            config = std::make_shared<Data::Config>(conf.string());
            LoadConfig();

            input_tables.clear();
            for (std::string input_file : BaseLoader::kInputFiles) {
                std::filesystem::path file = directory / input_file;
                if (!std::filesystem::exists(file)) {
                    continue;
                }
                input_tables[input_file] = ReadCSV(file.string(), true);
            }
        }

        Data::IConfigablePtr GetConfig() const { return config; }

        Data::IDataTablePtr LoadDataTable(const std::string &filename) {
            Data::IDataTablePtr dtp =
                std::make_shared<Data::DataTable>(filename);
            input_tables[filename] = dtp;
            return dtp;
        }

        std::vector<std::string> GetDemographicCombos() const {
            return demographic_combos;
        }

        void SetDemographicCombos(const std::vector<std::string> &combos) {
            demographic_combos = combos;
        }

        int GetAgeGroupShift() const { return age_group_shift; }

    protected:
        std::unordered_map<std::string, Data::IDataTablePtr> input_tables = {};
        Data::IConfigablePtr config;
        const std::string logger_name;

        // demographic
        int age_group_shift;
        std::vector<std::string> demographic_combos = {};

        // tabular files from the current RESPOND directory structure, as of
        // [2023-04-06]
        const std::vector<std::string> BaseLoader::kInputFiles = {
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

    private:
        void LoadConfig() {
            if (config) {
                respond::utils::LogError(
                    logger_name,
                    "Config not found when trying to load from config file.");
                return;
            }

            // Assumes ages are reported as XX_XX
            age_group_shift = CalculateAgeShift(
                config->getStringVector("demographic.age_groups"));

            BuildDemographicCombinations();
        }

        int CalculateAgeShift(const std::vector<std::string> &ages) {
            if (ages.empty()) {
                respond::utils::LogWarning(
                    logger_name, "No ages found when attempting to calculate "
                                 "Age Shift! Setting Age Shift to 0...");
                return 0;
            }
            std::string first_age_group = ages[0];
            std::string delimiter = "_";
            std::string first_age_string =
                first_age_group.substr(0, first_age_group.find(delimiter));
            int first_age = std::stoi(first_age_string);
            first_age_group.erase(0, first_age_string.length() +
                                         delimiter.length());
            int second_age = std::stoi(first_age_group);

            // have to add 1 for inclusive first value
            return (second_age - first_age) + 1;
        }

        void BuildDemographicCombinations() {
            std::vector<std::string> demographics =
                config->getSectionCategories("demographic");
            std::vector<std::vector<std::string>> running_demographics;
            for (std::string demographic : demographics) {
                running_demographics.push_back(
                    config->getStringVector("demographic." + demographic));
            }
            std::vector<std::vector<std::string>> temp1;
            std::vector<std::string> temp2;
            RecursionHelper(temp1, temp2, running_demographics.begin(),
                            running_demographics.end());
            std::stringstream group;
            for (std::vector<std::string> strList : temp1) {
                group.clear();
                for (std::string st : strList) {
                    std::transform(
                        st.begin(), st.end(), st.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    group << " " << st;
                }
                demographic_combos.push_back(group.str());
            }
        }

        void RecursionHelper(
            std::vector<std::vector<std::string>> &end_result,
            std::vector<std::string> &running_total,
            std::vector<std::vector<std::string>>::const_iterator current,
            std::vector<std::vector<std::string>>::const_iterator final) {
            if (current == final) {
                end_result.push_back(running_total);
                return;
            }
            const std::vector<std::string> &temp = *current;
            for (std::vector<std::string>::const_iterator it = temp.begin();
                 it != temp.end(); it++) {
                running_total.push_back(*it);
                RecursionHelper(end_result, running_total, current + 1, final);
                running_total.pop_back();
            }
        }
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_