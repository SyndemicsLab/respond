#ifndef CONTROLLER_CONFIGURATION_HPP_
#define CONTROLLER_CONFIGURATION_HPP_

#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;

static std::vector<std::string> parseStringVector(const std::string& inputText) {
    using boost::tokenizer;
    std::vector<std::string> toReturn;
    tokenizer<boost::escaped_list_separator<char>> token(inputText);
    for (tokenizer<boost::escaped_list_separator<char>>::iterator beg = token.begin();
         beg != token.end(); ++beg) {
        toReturn.push_back(*beg);
    }
    // remove trailing/leading whitespace
    for (std::string& value: toReturn) {
        boost::algorithm::trim(value);
    }
    return toReturn;
}

static std::vector<int> parseIntVector(const std::string& inputText) {
    using boost::tokenizer;
    std::vector<int> toReturn;
    std::vector<std::string> tmp;
    tokenizer<boost::escaped_list_separator<char>> token(inputText);
    for (tokenizer<boost::escaped_list_separator<char>>::iterator beg = token.begin();
         beg != token.end(); ++beg) {
        tmp.push_back(*beg);
    }
    // remove trailing/leading whitespace
    for (std::string& value: tmp) {
        boost::algorithm::trim(value);
        toReturn.push_back(stoi(value));
    }
    return toReturn;
}

struct Configuration {
public:
    Configuration() {};
    Configuration(const po::variables_map&);
    std::vector<std::string> interventions;
    std::vector<std::string> age_groups;
    std::vector<std::string> sex;
    std::vector<std::string> oud_states;
    int duration;
    int aging_interval;
    std::vector<int> intervention_change_times;
    std::vector<int> entering_sample_change_times;
    std::vector<int> overdose_change_times;
    bool cost_analysis;
    std::vector<std::string> cost_perspectives;
    double discount_rate;
    int reporting_interval;
    bool per_intervention_predictions;
    bool general_outputs;
    std::vector<int> general_stats_output_timesteps;
    bool cost_category_outputs;
};

Configuration configure(const std::string&);
#endif // CONTROLLER_CONFIGURATION_HPP_
