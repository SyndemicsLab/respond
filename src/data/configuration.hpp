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


class Configuration{
public:
    ~Configuration() {};
    Configuration() {};
    Configuration(std::string configFile);

    // getters
    std::vector<std::string> getInterventions();
    std::vector<std::string> getOUDStates();
    int getNumDemographicCombos();
    std::vector<int> getDemographicCounts();
    std::vector<int> getEnteringSampleChangeTimes();
    std::vector<int> getInterventionChangeTimes();
    std::vector<int> getOverdoseChangeTimes();
    std::vector<std::string> getDemographicCombos();

    int getAgingInterval();
    int getDuration();

    template<typename T>
    T get(std::string str);

private:
    std::map<std::string, std::vector<std::string>> demographicParams;
    std::vector<std::vector<std::string>> demographics;
    std::vector<std::string> demographicOrder;
    boost::property_tree::ptree ptree;

    void loadMap(std::pair<const std::string, boost::property_tree::ptree> section, std::map<std::string, std::string> m);
    std::vector<std::string> parseString2VectorOfStrings(std::string st);
    std::vector<int> parseString2VectorOfInts(std::string st);
};

#endif // DATA_CONFIGURATION_HPP_
