#include "configuration.hpp"

Configuration::Configuration(std::string configFile){
    read_ini(configFile, this->ptree);

    for (auto& section : this->ptree)
    {
        if(section.first.compare("demographic") == 0){
            for (auto& key : section.second){
                this->demographicOrder.push_back(key.first);
                this->demographicParams[key.first] = this->ParseString2VectorOfStrings(key.second.get_value<std::string>());
            }
        }
    }
}

/// @brief 
/// @return 
std::vector<std::string> Configuration::GetInterventions(){
    std::string res = this->ptree.get<std::string>("state.interventions");
    return this->ParseString2VectorOfStrings(res);
}

/// @brief 
/// @return 
std::vector<std::string> Configuration::GetOUDStates(){
    std::string res = this->ptree.get<std::string>("state.ouds");
    return this->ParseString2VectorOfStrings(res);
}


std::vector<int> updateIndices(std::vector<int> indices, std::vector<int> maxIndices){
    int lastIdx = indices.size()-1;
    std::vector<int> results = indices;
    results[lastIdx]++;
    for(int i = lastIdx; i > 0; i--){
        if((results[i] % maxIndices[i] == 0) && (results[i] != 0)){
            results[i] = 0;
            results[i-1]++;
        }
    }
    return results;

}

std::vector<std::string> Configuration::GetDemographicCombos(){
    int n = this->GetNumDemographicCombos();
    std::vector<int> demographics = this->GetDemographicCounts();
    int k = demographics.size();

    std::vector<int> indices(k, 0);

    std::vector<std::string> results;

    for(int i = 0; i < n; i++){
        std::string str;
        for(int j = 0; j < k; j++){
            str = str + " " + this->demographicParams[this->demographicOrder[j]][indices[j]];
        }
        results.push_back(str);
        indices = updateIndices(indices, demographics);
    }
    return results;
}




/// @brief 
/// @return 
int Configuration::GetNumDemographicCombos(){
    int totalCombos = 1;
    for(std::string key : this->demographicOrder){
        std::vector<std::string> temp = this->demographicParams[key];
        totalCombos *= temp.size();
    }
    return totalCombos;
}

void recurseHelper(std::vector<std::string> currentList){

}

/// @brief 
/// @return 
std::vector<int> Configuration::GetDemographicCounts(){
    std::vector<int> results;
    for(std::string key : this->demographicOrder){
        results.push_back(this->demographicParams[key].size());
    }
    return results;
}

/// @brief 
/// @return 
int Configuration::GetDuration(){
    return this->ptree.get<int>("simulation.duration");

}

std::vector<int> Configuration::GetEnteringSampleChangeTimes(){
    std::string res = this->ptree.get<std::string>("simulation.entering_sample_change_times");
    return this->ParseString2VectorOfInts(res);
}

std::vector<int> Configuration::GetInterventionChangeTimes(){
    std::string res = this->ptree.get<std::string>("simulation.intervention_change_times");
    return this->ParseString2VectorOfInts(res);
}

std::vector<int> Configuration::GetOverdoseChangeTimes(){
    std::string res = this->ptree.get<std::string>("simulation.overdose_change_times");
    return this->ParseString2VectorOfInts(res);
}


template<>
double Configuration::Get<double>(std::string str){
    return this->ptree.get<double>(str);
}

template<>
bool Configuration::Get<bool>(std::string str){
    return this->ptree.get<bool>(str);
}

template<>
int Configuration::Get<int>(std::string str){
    return this->ptree.get<int>(str);
}

template<>
std::vector<int> Configuration::Get<std::vector<int>>(std::string str){
    std::string ahh = this->ptree.get<std::string>(str);
    return this->ParseString2VectorOfInts(ahh);
}

template<>
std::vector<std::string> Configuration::Get<std::vector<std::string>>(std::string str){
    std::string res = this->ptree.get<std::string>(str);
    return this->ParseString2VectorOfStrings(res);
}


/// @brief 
/// @param section 
/// @param m 
void Configuration::LoadMap(std::pair<const std::string, boost::property_tree::ptree> section, std::map<std::string, std::string> m){
    for (auto& key : section.second){
        m[key.first] = key.second.get_value<std::string>();
    }
}

/// @brief 
/// @param st 
/// @return 
std::vector<std::string> Configuration::ParseString2VectorOfStrings(std::string st){
    std::stringstream ss( st );
    std::vector<std::string> result;

    while( ss.good() ){
        std::string substr;
        getline( ss, substr, ',' );
        int first = substr.find_first_not_of(' ');
        if(std::string::npos == first){ break; } // catch error and return result vector
        int last = substr.find_last_not_of(' ');
        result.push_back( substr.substr(first, (last-first+1)) );
    }
    return result;
}

/// @brief 
/// @param st 
/// @return 
std::vector<int> Configuration::ParseString2VectorOfInts(std::string st){

    std::vector<int> res;

    std::istringstream iss(st);
    std::string token;
    while (std::getline(iss, token, ',')){
         res.push_back(std::stoi(token));
    }
    if(res.empty()){ return {}; }
    return res;
}
