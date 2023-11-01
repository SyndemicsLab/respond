//===-- Configuration.cpp - Configuration class definition ------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Configuration class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Configuration.hpp"

namespace Data {

    /**************************************************
     *
     * Constructors
     *
     **************************************************/

    Configuration::Configuration() {
        this->demographicOrder = {};
        this->demographicParams = {};
    }

    Configuration::Configuration(std::string const &configFile) {
        std::ifstream f(configFile.c_str());
        ASSERTM(f.good(), "Valid Config File Provided");

        read_ini(configFile, this->ptree);

        for (auto &section : this->ptree) {
            if (section.first.compare("demographic") == 0) {
                for (auto &key : section.second) {
                    this->demographicOrder.push_back(key.first);
                    this->demographicParams[key.first] =
                        this->parseString2VectorOfStrings(
                            key.second.get_value<std::string>());
                }
            }
        }
    }

    /**************************************************
     *
     * Public Methods
     *
     **************************************************/

    std::vector<std::string> Configuration::getInterventions() {
        std::string res = this->ptree.get<std::string>("state.interventions");
        ASSERTM(!res.empty(), "Interventions Successfully Found");

        std::vector<std::string> result =
            this->parseString2VectorOfStrings(res);
        ASSERTM(result.size() % 2 != 0,
                "A valid, odd, number of interventions were provided");

        int mid = result.size() / 2 + 1;
        for (int i = 0; i < result.size() / 2; i++) {
            ASSERTM(
                result[mid + i].find(result[i + 1]) != std::string::npos,
                "Post Intervention order corresponds with Intervention Order.");
        }
        return result;
    }

    std::vector<std::string> Configuration::getOUDStates() {
        std::string res = this->ptree.get<std::string>("state.ouds");
        ASSERTM(!res.empty(), "OUDs Successfully Found");
        return this->parseString2VectorOfStrings(res);
    }

    std::vector<std::string> Configuration::getDemographicCombos() {
        int n = this->getNumDemographicCombos();
        std::vector<int> demographics = this->getDemographicCounts();
        ASSERTM(demographics.size() == this->demographicParams.size(),
                "Deomgraphic Parameter Sizes Match");

        int k = demographics.size();

        std::vector<int> indices(k, 0);
        std::vector<std::string> results;

        for (int i = 0; i < n; i++) {
            std::string str;
            for (int j = 0; j < k; j++) {
                str = str + " " +
                      this->demographicParams[this->demographicOrder[j]]
                                             [indices[j]];
            }
            results.push_back(str);
            indices = this->updateIndices(indices, demographics);
        }
        return results;
    }

    std::vector<std::vector<std::string>>
    Configuration::getDemographicCombosVecOfVec() {
        int n = this->getNumDemographicCombos();
        std::vector<int> demographics = this->getDemographicCounts();
        ASSERTM(demographics.size() == this->demographicParams.size(),
                "Deomgraphic Parameter Sizes Match");

        int k = demographics.size();

        std::vector<int> indices(k, 0);
        std::vector<std::vector<std::string>> results;

        for (int i = 0; i < n; i++) {
            std::vector<std::string> vec;
            for (int j = 0; j < k; j++) {
                vec.push_back(this->demographicParams[this->demographicOrder[j]]
                                                     [indices[j]]);
            }
            results.push_back(vec);
            indices = this->updateIndices(indices, demographics);
        }
        return results;
    }

    int Configuration::getNumDemographicCombos() {
        int totalCombos = 1;
        for (std::string key : this->demographicOrder) {
            std::vector<std::string> temp = this->demographicParams[key];
            totalCombos *= temp.size();
        }
        return totalCombos;
    }

    std::vector<int> Configuration::getDemographicCounts() {
        std::vector<int> results;
        for (std::string key : this->demographicOrder) {
            results.push_back(this->demographicParams[key].size());
        }
        return results;
    }

    int Configuration::getAgingInterval() {
        int aging = this->ptree.get<int>("simulation.aging_interval");
        ASSERTM(aging, "Aging Interval Successfully Found");
        return aging;
    }

    int Configuration::getDuration() {
        int duration = this->ptree.get<int>("simulation.duration");
        ASSERTM(duration, "Duration Successfully Found");
        return duration;
    }

    std::vector<int> Configuration::getEnteringSampleChangeTimes() {
        std::string res = this->ptree.get<std::string>(
            "simulation.entering_sample_change_times");
        std::vector<int> resVec = this->parseString2VectorOfInts(res);
        std::vector<int> result;
        for (int r : resVec) {
            ASSERTM(result.empty() || result.back() <= r,
                    "Cohort Change Times In Order");
            result.push_back(r - 1);
        }
        ASSERTM(this->getDuration() <= (result.back() + 1),
                "Acceptable Entering Cohort Change Time");

        return result;
    }

    std::vector<int> Configuration::getInterventionChangeTimes() {
        std::string res = this->ptree.get<std::string>(
            "simulation.intervention_change_times");
        std::vector<int> resVec = this->parseString2VectorOfInts(res);
        std::vector<int> result;
        for (int r : resVec) {
            ASSERTM(result.empty() || result.back() <= r,
                    "Intervention Change Times In Order");
            result.push_back(r - 1);
        }

        ASSERTM(this->getDuration() <= (result.back() + 1),
                "Acceptable Intervention Change Time");

        return result;
    }

    std::vector<int> Configuration::getOverdoseChangeTimes() {
        std::string res =
            this->ptree.get<std::string>("simulation.overdose_change_times");
        std::vector<int> resVec = this->parseString2VectorOfInts(res);
        std::vector<int> result;
        for (int r : resVec) {
            ASSERTM(result.empty() || result.back() <= r,
                    "Overdose Change Times In Order");
            result.push_back(r - 1);
        }

        ASSERTM(this->getDuration() <= (result.back() + 1),
                "Acceptable Overdose Change Time");

        return result;
    }

    bool Configuration::getCostSwitch() {
        return this->ptree.get<bool>("cost.cost_analysis");
    }

    std::vector<std::string> Configuration::getCostPerspectives() {
        std::string res =
            this->ptree.get<std::string>("cost.cost_perspectives");
        ASSERTM(!res.empty(), "Cost Perspectives Successfully Provided.");
        std::vector<std::string> result =
            this->parseString2VectorOfStrings(res);
        return result;
    }

    double Configuration::getDiscountRate() {
        return this->ptree.get<double>("cost.discount_rate");
    }

    int Configuration::getReportingInterval() {
        return this->ptree.get<int>("cost.reporting_interval");
    }

    bool Configuration::getCostCategoryOutputs() {
        return this->ptree.get<bool>("cost.cost_category_outputs");
    }

    bool Configuration::getPerInterventionPredictions() {
        return this->ptree.get<bool>("output.per_intervention_predictions");
    }

    bool Configuration::getGeneralOutputsSwitch() {
        return this->ptree.get<bool>("output.general_outputs");
    }

    std::vector<int> Configuration::getGeneralStatsOutputTimesteps() {
        std::vector<int> result;
        std::string res = this->ptree.get<std::string>(
            "output.general_stats_output_timesteps");
        ASSERTM(!res.empty(), "General Stat Timesteps Successfully Provided.");

        std::vector<int> resVec = this->parseString2VectorOfInts(res);
        for (int r : resVec) {
            result.push_back(r);
        }

        ASSERTM(this->getDuration() <= (result.back() + 1),
                "Successful General Stats Output Timesteps.");

        return result;
    }

    template <> double Configuration::get<double>(std::string str) {
        return this->ptree.get<double>(str);
    }

    template <> bool Configuration::get<bool>(std::string str) {
        return this->ptree.get<bool>(str);
    }

    template <> int Configuration::get<int>(std::string str) {
        return this->ptree.get<int>(str);
    }

    template <>
    std::vector<int> Configuration::get<std::vector<int>>(std::string str) {
        std::string res = this->ptree.get<std::string>(str);
        std::vector<int> resVec = this->parseString2VectorOfInts(res);
        std::vector<int> result;
        for (int r : resVec) {
            result.push_back(r - 1);
        }
        return result;
    }

    template <>
    std::vector<std::string>
    Configuration::get<std::vector<std::string>>(std::string str) {
        std::string res = this->ptree.get<std::string>(str);
        if (res.empty()) {
            return {};
        }
        return this->parseString2VectorOfStrings(res);
    }

    /**************************************************
     *
     * Private Methods
     *
     **************************************************/
    std::vector<int> Configuration::updateIndices(std::vector<int> indices,
                                                  std::vector<int> maxIndices) {
        int lastIdx = indices.size() - 1;
        ASSERTM(lastIdx >= 0, "Valid Index Count");

        std::vector<int> results = indices;
        results[lastIdx]++;
        for (int i = lastIdx; i > 0; i--) {
            if ((results[i] % maxIndices[i] == 0) && (results[i] != 0)) {
                results[i] = 0;
                results[i - 1]++;
            }
        }
        return results;
    }

    std::vector<std::string>
    Configuration::parseString2VectorOfStrings(std::string st) {
        std::stringstream ss(st);
        std::vector<std::string> result;

        while (ss.good()) {
            std::string substr;
            getline(ss, substr, ',');
            int first = substr.find_first_not_of(' ');
            if (std::string::npos == first) {
                break;
            } // catch error and return result vector
            int last = substr.find_last_not_of(' ');
            result.push_back(substr.substr(first, (last - first + 1)));
        }
        return result;
    }

    std::vector<int> Configuration::parseString2VectorOfInts(std::string st) {
        std::vector<int> res;

        std::istringstream iss(st);
        std::string token;
        while (std::getline(iss, token, ',')) {
            res.push_back(std::stoi(token));
        }
        if (res.empty()) {
            return {};
        }
        return res;
    }
} // namespace Data