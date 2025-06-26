////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: constants                                                         //
// Created Date: 2025-06-26                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_CONSTANTS_CONFIG_HPP_
#define RESPOND_TESTS_CONSTANTS_CONFIG_HPP_

#include <fstream>
#include <sstream>
#include <string>

namespace respond {
namespace testing {
inline std::string SectionBreak() {
    std::stringstream ss;
    ss << std::endl << std::endl;
    return ss.str();
}

inline void BuildSimConf(const std::string &name) {
    std::stringstream ss;
    ss << "[simulation]" << std::endl
       << "duration = 52" << std::endl
       << "aging_interval = 260" << std::endl
       << "intervention_change_times = 52" << std::endl
       << "entering_sample_change_times = 52" << std::endl
       << "overdose_change_times = 52" << std::endl
       << "stratified_entering_cohort = false" << SectionBreak();
    ss << "[state]" << std::endl
       << "interventions = No_Treatment, Buprenorphine,Naltrexone, Methadone, "
       << "Detox, Post-Buprenorphine,Post-Naltrexone, Post-Methadone, "
       << "Post-Detox" << std::endl
       << "behaviors = Active_Noninjection, Active_Injection,"
          "Nonactive_Noninjection, Nonactive_Injection"
       << SectionBreak();
    ss << "[demographic]" << std::endl
       << "age_groups = 10_14, 15_19, 20_24, 25_29, 30_34, 35_39, 40_44, "
          "45_49, 50_54, 55_59, 60_64, 65_69, 70_74, 75_79, 80_84, 85_89, "
          "90_94, 95_99 "
       << std::endl
       << "sex = Male, Female " << SectionBreak();
    ss << "[cost]" << std::endl
       << "cost_analysis = true" << std::endl
       << "cost_perspectives = healthcare" << std::endl
       << "discount_rate = 0.0025 " << std::endl
       << "reporting_interval = 1" << std::endl
       << "cost_utility_output_timesteps = 52 " << std::endl
       << "cost_category_outputs = false " << SectionBreak();
    ss << "[output] " << std::endl
       << "per_intervention_predictions = true " << std::endl
       << "general_outputs = false " << std::endl
       << "general_stats_output_timesteps = 52";
    std::ofstream f(name);
    f << ss.str();
    f.close();
}
} // namespace testing
} // namespace respond

#endif