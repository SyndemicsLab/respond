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

namespace Data{

    class IConfiguration{
    public:
        virtual ~IConfiguration(){};
        virtual std::vector<std::string> getInterventions() = 0;
        virtual std::vector<std::string> getOUDStates() = 0;
        virtual int getNumDemographicCombos() = 0;
        virtual std::vector<int> getDemographicCounts() = 0;
        virtual std::vector<int> getEnteringSampleChangeTimes() = 0;
        virtual std::vector<int> getInterventionChangeTimes() = 0;
        virtual std::vector<int> getOverdoseChangeTimes() = 0;
        virtual std::vector<std::string> getDemographicCombos() = 0;
        virtual int getAgingInterval() = 0;
        virtual int getDuration() = 0;
    };

    /// @brief 
    class Configuration : public IConfiguration {
    public:
        ~Configuration() {};
        Configuration();
        Configuration(std::string configFile);

        /// @brief 
        /// @return 
        std::vector<std::string> getInterventions() override;

        /// @brief 
        /// @return 
        std::vector<std::string> getOUDStates() override;

        /// @brief 
        /// @return 
        int getNumDemographicCombos() override;

        /// @brief 
        /// @return 
        std::vector<int> getDemographicCounts() override;

        /// @brief 
        /// @return 
        std::vector<int> getEnteringSampleChangeTimes() override;

        /// @brief 
        /// @return 
        std::vector<int> getInterventionChangeTimes() override;

        /// @brief 
        /// @return 
        std::vector<int> getOverdoseChangeTimes() override;

        /// @brief 
        /// @return 
        std::vector<std::string> getDemographicCombos() override;

        /// @brief 
        /// @return 
        int getAgingInterval() override;

        /// @brief 
        /// @return 
        int getDuration() override;

        /// @brief 
        /// @tparam T 
        /// @param str 
        /// @return 
        template<typename T>
        T get(std::string str);

    private:
        std::map<std::string, std::vector<std::string>> demographicParams;
        std::vector<std::string> demographicOrder;
        boost::property_tree::ptree ptree;

        std::vector<std::string> parseString2VectorOfStrings(std::string st);
        std::vector<int> parseString2VectorOfInts(std::string st);
        std::vector<int> updateIndices(std::vector<int> indices, std::vector<int> maxIndices);
    };
}

#endif // DATA_CONFIGURATION_HPP_
