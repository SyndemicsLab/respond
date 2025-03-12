////////////////////////////////////////////////////////////////////////////////
// File: base_loader_internals.hpp                                            //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_BASELOADERINTERNALS_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <datamanagement/DataManagement.hpp>
#include <spdlog/spdlog.h>

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

            std::filesystem::path inputPath = directory;
            this->ReadInputDirectory(directory);
        }

        Data::IDataTablePtr ReadCSV(const std::string &path,
                                    bool headers = true) {
            Data::IDataTablePtr table =
                std::make_shared<Data::DataTable>(path, headers, ',');
            return table;
        }

        void ReadInputDirectory(const std::filesystem::path &directory) {
            std::filesystem::path conf = directory / "sim.conf";
            this->config = std::make_shared<Data::Config>(conf.string());
            LoadConfig();

            std::unordered_map<std::string, Data::IDataTablePtr> toReturn;
            input_tables.clear();

            for (std::string input_file : BaseLoader::kInputFiles) {
                std::filesystem::path file = directory / input_file;
                if (!std::filesystem::exists(file)) {
                    continue;
                }
                input_tables[input_file] = ReadCSV(file.string(), true);
            }
        }

        virtual Data::IConfigablePtr GetConfig() const = 0;

        virtual Data::IDataTablePtr
        LoadDataTable(const std::string &filename) = 0;

        virtual std::vector<std::string> GetDemographicCombos() const = 0;

        virtual void SetDemographicCombos(std::vector<std::string> combos) = 0;

        virtual int GetAgeGroupShift() const = 0;

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
            if (!this->config) {
                respond::utils::LogDebug(
                    logger_name,
                    "Config not found when trying to load from config file.");
                return;
            }

            // demographic
            // age_group_shift Calculation. Assumes ages are reported as XX_XX
            std::vector<std::string> ages =
                this->config->getStringVector("demographic.age_groups");

            age_group_shift = CalculateAgeShift(ages);

            std::vector<std::string> demographics =
                config->getSectionCategories("demographic");
            std::vector<std::vector<std::string>> inputDemographicVals;
            for (std::string demographic : demographics) {
                inputDemographicVals.push_back(this->config->getStringVector(
                    "demographic." + demographic));
            }
            std::vector<std::vector<std::string>> temp1;
            std::vector<std::string> temp2;
            RecursionHelper(temp1, temp2, inputDemographicVals.begin(),
                            inputDemographicVals.end());
            for (std::vector<std::string> strList : temp1) {
                std::string group = "";
                for (std::string st : strList) {
                    std::transform(
                        st.begin(), st.end(), st.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    group += " " + st;
                }
                this->demographic_combos.push_back(group);
            }
        }

        int CalculateAgeShift(const std::vector<std::string> &ages) {
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