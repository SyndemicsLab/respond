#include "configuration.hpp"

Configuration::Configuration(std::string configFile){
    read_ini(configFile, this->ptree);

    for (auto& section : this->ptree)
    {
        if(section.first.compare("demographic")){
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
    return this->ParseString2VectorOfStrings(this->ptree.get<std::string>("state.interventions"));
}

/// @brief 
/// @return 
std::vector<std::string> Configuration::GetOUDStates(){
    return this->ParseString2VectorOfStrings(this->ptree.get<std::string>("state.ouds"));
}

/// @brief 
/// @return 
std::vector<std::vector<std::string>> Configuration::GetDemographicCombos(){
    std::vector<std::vector<std::string>> dems;
    for(std::string key : this->demographicOrder){
        dems.push_back(this->demographicParams[key]);
    }

    // number of arrays
    size_t n = dems.size();
 
    std::vector<std::vector<std::string>> demographicCombo;
    // to keep track of next element in each of
    // the n arrays
    std::vector<int> indices(n, 0);

    int next = n - 1;
    while (next >= 0) {
        
        std::vector<std::string> combination(n);
        // print current combination
        for (int i = 0; i < n; i++){
            combination.push_back(dems[i][indices[i]]);
        }
        demographicCombo.push_back(combination);
 
        // find the rightmost array that has more
        // elements left after the current element
        // in that array
        next = n - 1;
        while (next >= 0 && (indices[next] + 1 >= dems[next].size())){
            next--;
        }
 
        // no such array is found so no more
        // combinations left
        if (next < 0)
            break;
 
        // if found move to next element in that
        // array
        indices[next]++;
 
        // for all arrays to the right of this
        // array current index again points to
        // first element
        for (int i = next + 1; i < n; i++){
            indices[i] = 0;
        }
    }    
    return demographicCombo;
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
uint16_t Configuration::GetDuration(){
    return this->ptree.get<uint16_t>("simulation.duration");

}

std::vector<int> Configuration::GetEnteringSampleChangeTimes(){
    return this->ParseString2VectorOfInts(this->ptree.get<std::string>("simulation.entering_sample_change_times"));
}

std::vector<int> Configuration::GetInterventionChangeTimes(){
    return this->ParseString2VectorOfInts(this->ptree.get<std::string>("state.intervention_change_times"));
}

std::vector<int> Configuration::GetOverdoseChangeTimes(){
    return this->ParseString2VectorOfInts(this->ptree.get<std::string>("simulation.overdose_change_times"));
}


template<>
double Configuration::Get<double>(const std::string& str){
    return this->ptree.get<double>(str);
}

template<>
bool Configuration::Get<bool>(const std::string& str){
    return this->ptree.get<bool>(str);
}

template<>
int Configuration::Get<int>(const std::string& str){
    return this->ptree.get<int>(str);
}

template<>
std::vector<int> Configuration::Get<std::vector<int>>(const std::string& str){
    std::string ahh = this->ptree.get<std::string>(str);
    return this->ParseString2VectorOfInts(ahh);
}

template<>
std::vector<std::string> Configuration::Get<std::vector<std::string>>(const std::string& str){
    return this->ParseString2VectorOfStrings(this->ptree.get<std::string>(str));
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
        result.push_back( substr );
    }
    return result;
}

/// @brief 
/// @param st 
/// @return 
std::vector<int> Configuration::ParseString2VectorOfInts(std::string st){
    std::stringstream iss( st );

    int number;
    std::vector<int> result;
    while(iss.good()){
        std::string substr;
        getline(iss, substr, ',');
        result.push_back(std::stoi(substr));
    }
    return result;
}
