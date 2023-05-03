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

    /// @brief An object that stores parameters from model configuration files
    /// @details `RESPOND`, by default, searches for a file named `sim.conf` from
    /// within a given input directory to define simulation initialization
    /// conditions.
    ///
    /// `RESPOND` is written with flexibility in mind. As such, the configuration
    /// file allows for a significant amount of flexibility. Consider the
    /// following example `sim.conf`:
    /// @code{.ini}
    /// [simulation]
    /// duration = 156
    /// aging_interval = 260
    /// intervention_change_times = 52, 104, 156
    /// entering_sample_change_times = 52, 104, 156
    /// overdose_change_times = 52, 104, 156
    ///
    /// [state]
    /// interventions = No_Treatment, Buprenorphine, Naltrexone, Post-Buprenorphine, Post-Naltrexone
    /// ouds = Active_Noninjection, Active_Injection, Nonactive_Noninjection, Nonactive_Injection
    ///
    /// [demographic]
    /// age_groups = 10_14, 15_19, 20_24
    /// sex = male, female
    ///
    /// [cost]
    /// cost_analysis = true
    /// cost_perspectives = Healthcare System, Societal, Policymakers
    /// discount_rate = 0.0025
    ///
    /// [output]
    /// reporting_interval = 2
    /// per_intervention_predictions = true
    /// general_outputs = false
    /// general_stats_output_timesteps = 52, 104, 156
    /// cost_category_outputs = false
    /// @endcode
    ///
    /// This configuration is broken down into sections `[simulation]`, `[state]`,
    /// `[demographic]`, `[cost]`, and `[output]`. Each of these sections is a
    /// top-level key in a `boost::property_tree` object `Configuration.ptree`.
    /// Each of the parameters, e.g. `interventions`, `ouds`, `cost_analysis`,
    /// is also a key-value pair that represents an individual model input
    /// parameter. This design enables parameters to be used either
    /// individually, e.g. `state.interventions`, or as a section, e.g.
    /// `cost`.
    ///
    /// **Required Keys:**
    ///
    /// Individual parameters are presented in the form `paramName`/`valueType`
    /// - `[simulation]`
    ///   - `duration`/`int`
    ///   - `intervention_change_times`/`std::vector<int>`
    ///   - `entering_sample_change_times`/`std::vector<int>`
    ///   - `overdose_change_times`/`std::vector<int>`
    /// - `[state]`
    ///   - `interventions`/`std::vector<string>`
    ///   - `ouds`/`std::vector<string>`
    /// - `[demographic]`
    ///   - `age_groups`/`std::vector<string>`
    ///   - Varies*/`std::vector<string>`
    ///
    /// *The section `[demographic]` is currently (as of **[2023-05-01]**) one
    /// place where there is a limitation to the flexibility of `RESPOND` based
    /// on functionality. Because the population is capable of "aging", the
    /// `age_group` parameter **MUST** be specified first in `[demographic]`.
    /// Beyond this, `[demographic]` is capable of supporting an unbounded number
    /// of demographic vectors.
    class Configuration : public IConfiguration {
    public:
        // CONSTRUCTORS
        Configuration();
        Configuration(std::string configFile);
        ~Configuration() {};

        // GENERATOR FUNCTIONS
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
