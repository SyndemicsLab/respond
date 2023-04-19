#ifndef DATA_CONFIGURATION_HPP_
#define DATA_CONFIGURATION_HPP_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <any>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>


template <typename C> struct is_vector : std::false_type {};    
template <typename T,typename A> struct is_vector< std::vector<T,A> > : std::true_type {};    
template <typename C> inline constexpr bool is_vector_v = is_vector<C>::value;

class Configuration{
public:
    Configuration() {};
    Configuration(std::string configFile);

    // getters
    std::vector<std::string> GetInterventions();
    std::vector<std::string> GetOUDStates();
    std::vector<std::vector<std::string>> GetDemographicCombos();
    std::vector<int> GetDemographicCounts();
    std::vector<int> GetEnteringSampleChangeTimes();
    std::vector<int> GetInterventionChangeTimes();
    std::vector<int> GetOverdoseChangeTimes();

    uint16_t GetDuration();

    template<typename T>
    T Get(const std::string& str);

private:
    std::map<std::string, std::vector<std::string>> demographicParams;
    std::vector<std::string> demographicOrder;
    boost::property_tree::ptree ptree;

    void LoadMap(std::pair<const std::string, boost::property_tree::ptree> section, std::map<std::string, std::string> m);
    std::vector<std::string> ParseString2VectorOfStrings(std::string st);
    std::vector<int> ParseString2VectorOfInts(std::string st);
};

#endif // DATA_CONFIGURATION_HPP_
