#include "Configuration.hpp"

using namespace Data;

/**************************************************
 *
 * Constructors
 *
 **************************************************/

Configuration::Configuration() {
    this->demographicOrder = {};
    this->demographicParams = {};
}

Configuration::Configuration(std::string configFile) {
    std::ifstream f(configFile.c_str());
    if(!f.good()){
        throw std::invalid_argument("Invalid Config File Provided. " + configFile + " was not found.");
    }

    read_ini(configFile, this->ptree);

    for (auto& section : this->ptree) {
        if(section.first.compare("demographic") == 0) {
            for (auto& key : section.second) {
                this->demographicOrder.push_back(key.first);
                this->demographicParams[key.first] = this->parseString2VectorOfStrings(key.second.get_value<std::string>());
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
    if(res.empty()){ 
        throw std::invalid_argument("No Valid Interventions Provided. Make sure state.interventions exists in your config file!");
    }
    std::vector<std::string> result = this->parseString2VectorOfStrings(res);

    if(result.size()%2 == 0){
        throw std::invalid_argument("An even number of interventions were provided. "
        "The model assumes there is one No Treatment Intervention and then a Post Treatment for each treatment provided.");
    }
    int mid = result.size()/2 + 1;
    for(int i=0; i < result.size()/2; i++){
        if(result[mid+i].find(result[i+1]) == std::string::npos){
            throw std::invalid_argument("Post-Intervention order does not correspond with Intervention Order.");
        }
    }
    return result;
}

std::vector<std::string> Configuration::getOUDStates() {
    std::string res = this->ptree.get<std::string>("state.ouds");
    if(res.empty()){
        throw std::invalid_argument("No Valid OUDs Provided. Make sure state.ouds exists in your config file!");
    }
    return this->parseString2VectorOfStrings(res);
}

std::vector<std::string> Configuration::getDemographicCombos() {
    int n = this->getNumDemographicCombos();
    std::vector<int> demographics = this->getDemographicCounts();
    if(demographics.size() != this->demographicParams.size()){
        throw std::invalid_argument("The Demographic Parameters size do not match.");
    }
    int k = demographics.size();

    std::vector<int> indices(k, 0);
    std::vector<std::string> results;

    for(int i = 0; i < n; i++) {
        std::string str;
        for(int j = 0; j < k; j++) {
            str = str + " " + this->demographicParams[this->demographicOrder[j]][indices[j]];
        }
        results.push_back(str);
        indices = this->updateIndices(indices, demographics);
    }
    return results;
}

std::vector<std::vector<std::string>> Configuration::getDemographicCombosVecOfVec() {
    int n = this->getNumDemographicCombos();
    std::vector<int> demographics = this->getDemographicCounts();
    if(demographics.size() != this->demographicParams.size()){
        throw std::invalid_argument("The Demographic Parameters size do not match.");
    }
    int k = demographics.size();

    std::vector<int> indices(k, 0);
    std::vector<std::vector<std::string>> results;

    for(int i = 0; i < n; i++) {
        std::vector<std::string> vec;
        for(int j = 0; j < k; j++) {
            vec.push_back(this->demographicParams[this->demographicOrder[j]][indices[j]]);
        }
        results.push_back(vec);
        indices = this->updateIndices(indices, demographics);
    }
    return results;
}

int Configuration::getNumDemographicCombos() {
    int totalCombos = 1;
    for(std::string key : this->demographicOrder) {
        std::vector<std::string> temp = this->demographicParams[key];
        totalCombos *= temp.size();
    }
    return totalCombos;
}

std::vector<int> Configuration::getDemographicCounts() {
    std::vector<int> results;
    for(std::string key : this->demographicOrder) {
        results.push_back(this->demographicParams[key].size());
    }
    return results;
}

int Configuration::getAgingInterval() {
    return this->ptree.get<int>("simulation.aging_interval");
}

int Configuration::getDuration() {
    int duration = this->ptree.get<int>("simulation.duration");
    if(duration <= 0){
        throw std::invalid_argument("Invalid Duration Provided");
    }
    return duration;

}

std::vector<int> Configuration::getEnteringSampleChangeTimes() {
    std::string res = this->ptree.get<std::string>("simulation.entering_sample_change_times");
    std::vector<int> resVec = this->parseString2VectorOfInts(res);
    std::vector<int> result;
    for(int r : resVec){
        if(!result.empty() && result.back() > r){
            throw std::invalid_argument("Invalid Entering Cohort Change Times. Values are out of order.");
        }
        result.push_back(r-1);
    }

    if(this->getDuration() > (result.back()+1)){
        throw std::invalid_argument("Invalid Entering Cohort Change Times. Last value must be greater than or equal to the duration");
    }

    return result;
}

std::vector<int> Configuration::getInterventionChangeTimes() {
    std::string res = this->ptree.get<std::string>("simulation.intervention_change_times");
    std::vector<int> resVec = this->parseString2VectorOfInts(res);
    std::vector<int> result;
    for(int r : resVec){
        if(!result.empty() && result.back() > r){
            throw std::invalid_argument("Invalid Intervention Change Times. Values are out of order.");
        }
        result.push_back(r-1);
    }

    if(this->getDuration() > (result.back()+1)){
        throw std::invalid_argument("Invalid Intervention Change Times. Last value must be greater than or equal to the duration");
    }

    return result;
}

std::vector<int> Configuration::getOverdoseChangeTimes() {
    std::string res = this->ptree.get<std::string>("simulation.overdose_change_times");
    std::vector<int> resVec = this->parseString2VectorOfInts(res);
    std::vector<int> result;
    for(int r : resVec){
        if(!result.empty() && result.back() > r){
            throw std::invalid_argument("Invalid Overdose Change Times. Values are out of order.");
        }
        result.push_back(r-1);
    }

    if(this->getDuration() > (result.back()+1)){
        throw std::invalid_argument("Invalid Overdose Change Times. Last value must be greater than or equal to the duration.");
    }

    return result;
}

template<>
double Configuration::get<double>(std::string str) {
    return this->ptree.get<double>(str);
}

template<>
bool Configuration::get<bool>(std::string str) {
    return this->ptree.get<bool>(str);
}

template<>
int Configuration::get<int>(std::string str) {
    return this->ptree.get<int>(str);
}

template<>
std::vector<int> Configuration::get<std::vector<int>>(std::string str) {
    std::string res = this->ptree.get<std::string>(str);
    std::vector<int> resVec = this->parseString2VectorOfInts(res);
    std::vector<int> result;
    for(int r : resVec){
        result.push_back(r-1);
    }
    return result;
}

template<>
std::vector<std::string> Configuration::get<std::vector<std::string>>(std::string str) {
    std::string res = this->ptree.get<std::string>(str);
    if(res.empty()){ return {}; }
    return this->parseString2VectorOfStrings(res);
}


/**************************************************
 *
 * Private Methods
 *
 **************************************************/

/// @brief
/// @param indices
/// @param maxIndices
/// @return
std::vector<int> Configuration::updateIndices(std::vector<int> indices, std::vector<int> maxIndices) {
    int lastIdx = indices.size()-1;
    if(lastIdx < 0){
        throw std::invalid_argument("Invalid index count");
    }
    std::vector<int> results = indices;
    results[lastIdx]++;
    for(int i = lastIdx; i > 0; i--) {
        if((results[i] % maxIndices[i] == 0) && (results[i] != 0)) {
            results[i] = 0;
            results[i-1]++;
        }
    }
    return results;

}

/// @brief
/// @param st
/// @return
std::vector<std::string> Configuration::parseString2VectorOfStrings(std::string st) {
    std::stringstream ss( st );
    std::vector<std::string> result;

    while( ss.good() ) {
        std::string substr;
        getline( ss, substr, ',' );
        int first = substr.find_first_not_of(' ');
        if(std::string::npos == first) { break; } // catch error and return result vector
        int last = substr.find_last_not_of(' ');
        result.push_back( substr.substr(first, (last-first+1)) );
    }
    return result;
}

/// @brief
/// @param st
/// @return
std::vector<int> Configuration::parseString2VectorOfInts(std::string st) {
    std::vector<int> res;

    std::istringstream iss(st);
    std::string token;
    while (std::getline(iss, token, ',')) {
        res.push_back(std::stoi(token));
    }
    if(res.empty()) { return {}; }
    return res;
}
