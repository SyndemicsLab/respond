////////////////////////////////////////////////////////////////////////////////
// File: expected_strings.hpp                                                 //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-25                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TESTS_MOCKS_EXPECTEDSTRINGS_HPP_
#define RESPOND_TESTS_MOCKS_EXPECTEDSTRINGS_HPP_

#include <sstream>
#include <string>

namespace respond::tests {
class ExpectedStrings {
private:
    std::string LoadWriteInputDataStream() const {
        std::stringstream stream;
        stream
            << "intervention,agegrp,race,sex,initial_oud,Active_Noninjection,"
               "Active_Injection,Nonactive_Noninjection,Nonactive_Injection"
            << std::endl
            << "No_Treatment,1_100,other,Active_Noninjection,0.900000,0.900000,"
               "0.900000,0.900000"
            << std::endl
            << "Buprenorphine,1_100,other,Active_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Naltrexone,1_100,other,Active_Noninjection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Methadone,1_100,other,Active_Noninjection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Detox,1_100,other,Active_Noninjection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Active_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Active_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Methadone,1_100,other,Active_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Detox,1_100,other,Active_Noninjection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "No_Treatment,1_100,other,Active_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Buprenorphine,1_100,other,Active_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Naltrexone,1_100,other,Active_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Methadone,1_100,other,Active_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Detox,1_100,other,Active_Injection,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Active_Injection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Active_Injection,0.900000,0.900000,"
               "0.900000,0.900000"
            << std::endl
            << "Post-Methadone,1_100,other,Active_Injection,0.900000,0.900000,"
               "0.900000,0.900000"
            << std::endl
            << "Post-Detox,1_100,other,Active_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "No_Treatment,1_100,other,Nonactive_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Buprenorphine,1_100,other,Nonactive_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Naltrexone,1_100,other,Nonactive_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Methadone,1_100,other,Nonactive_Noninjection,0.900000,0.900000,"
               "0.900000,0.900000"
            << std::endl
            << "Detox,1_100,other,Nonactive_Noninjection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Nonactive_Noninjection,0.900000,"
               "0.900000,0.900000,0.900000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Nonactive_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Methadone,1_100,other,Nonactive_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Detox,1_100,other,Nonactive_Noninjection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "No_Treatment,1_100,other,Nonactive_Injection,0.900000,0.900000,"
               "0.900000,0.900000"
            << std::endl
            << "Buprenorphine,1_100,other,Nonactive_Injection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Naltrexone,1_100,other,Nonactive_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Methadone,1_100,other,Nonactive_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Detox,1_100,other,Nonactive_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Nonactive_Injection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Nonactive_Injection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Methadone,1_100,other,Nonactive_Injection,0.900000,0."
               "900000,0.900000,0.900000"
            << std::endl
            << "Post-Detox,1_100,other,Nonactive_Injection,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "agegrp,race,sex,oud,initial_intervention,No_Treatment_1_52,"
               "Buprenorphine_1_52,Naltrexone_1_52,Methadone_1_52,Detox_1_52,"
               "Post-Buprenorphine_1_52,Post-Naltrexone_1_52,Post-Methadone_1_"
               "52,Post-Detox_1_52"
            << std::endl
            << "1_100,other,Active_Noninjection,No_Treatment,0.800000,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Buprenorphine,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Naltrexone,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Methadone,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Detox,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Post-Buprenorphine,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Post-Naltrexone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Post-Methadone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Active_Noninjection,Post-Detox,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,No_Treatment,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Buprenorphine,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Naltrexone,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Methadone,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Detox,0.800000,0.800000,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Post-Buprenorphine,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Active_Injection,Post-Naltrexone,0.800000,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Post-Methadone,0.800000,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Active_Injection,Post-Detox,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,No_Treatment,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Buprenorphine,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Naltrexone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Methadone,0.800000,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Detox,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Post-Buprenorphine,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,"
               "0.800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Post-Naltrexone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Post-Methadone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Noninjection,Post-Detox,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,No_Treatment,0.800000,0.800000,"
               "0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Buprenorphine,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Naltrexone,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Methadone,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Detox,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Post-Buprenorphine,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Post-Naltrexone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Post-Methadone,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000,0."
               "800000"
            << std::endl
            << "1_100,other,Nonactive_Injection,Post-Detox,0.800000,0.800000,0."
               "800000,0.800000,0.800000,0.800000,0.800000,0.800000,0.800000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "initial_oud_state,to_intervention,Active_Noninjection,Active_"
               "Injection,Nonactive_Noninjection,Nonactive_Injection"
            << std::endl
            << "Active_Noninjection,No_Treatment,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Noninjection,Buprenorphine,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Noninjection,Naltrexone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Noninjection,Methadone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Noninjection,Detox,0.900000,0.900000,0.900000,0.900000"
            << std::endl
            << "Active_Noninjection,Post-Buprenorphine,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Active_Noninjection,Post-Naltrexone,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Active_Noninjection,Post-Methadone,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Active_Noninjection,Post-Detox,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Injection,No_Treatment,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Injection,Buprenorphine,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Injection,Naltrexone,0.900000,0.900000,0.900000,0.900000"
            << std::endl
            << "Active_Injection,Methadone,0.900000,0.900000,0.900000,0.900000"
            << std::endl
            << "Active_Injection,Detox,0.900000,0.900000,0.900000,0.900000"
            << std::endl
            << "Active_Injection,Post-Buprenorphine,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Active_Injection,Post-Naltrexone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Injection,Post-Methadone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Active_Injection,Post-Detox,0.900000,0.900000,0.900000,0.900000"
            << std::endl
            << "Nonactive_Noninjection,No_Treatment,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Nonactive_Noninjection,Buprenorphine,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Nonactive_Noninjection,Naltrexone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Noninjection,Methadone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Noninjection,Detox,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Noninjection,Post-Buprenorphine,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Nonactive_Noninjection,Post-Naltrexone,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Nonactive_Noninjection,Post-Methadone,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Nonactive_Noninjection,Post-Detox,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Injection,No_Treatment,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Injection,Buprenorphine,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Injection,Naltrexone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Injection,Methadone,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "Nonactive_Injection,Detox,0.900000,0.900000,0.900000,0.900000"
            << std::endl
            << "Nonactive_Injection,Post-Buprenorphine,0.900000,0.900000,0."
               "900000,0.900000"
            << std::endl
            << "Nonactive_Injection,Post-Naltrexone,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Nonactive_Injection,Post-Methadone,0.900000,0.900000,0.900000,"
               "0.900000"
            << std::endl
            << "Nonactive_Injection,Post-Detox,0.900000,0.900000,0.900000,0."
               "900000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "intervention,agegrp,race,sex,oud,overdose_1_52" << std::endl
            << "No_Treatment,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "No_Treatment,1_100,other,Active_Injection,0.700000" << std::endl
            << "No_Treatment,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "No_Treatment,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "Buprenorphine,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "Buprenorphine,1_100,other,Active_Injection,0.700000"
            << std::endl
            << "Buprenorphine,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Buprenorphine,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "Naltrexone,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "Naltrexone,1_100,other,Active_Injection,0.700000" << std::endl
            << "Naltrexone,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Naltrexone,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "Methadone,1_100,other,Active_Noninjection,0.700000" << std::endl
            << "Methadone,1_100,other,Active_Injection,0.700000" << std::endl
            << "Methadone,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Methadone,1_100,other,Nonactive_Injection,0.700000" << std::endl
            << "Detox,1_100,other,Active_Noninjection,0.700000" << std::endl
            << "Detox,1_100,other,Active_Injection,0.700000" << std::endl
            << "Detox,1_100,other,Nonactive_Noninjection,0.700000" << std::endl
            << "Detox,1_100,other,Nonactive_Injection,0.700000" << std::endl
            << "Post-Buprenorphine,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Active_Injection,0.700000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Post-Buprenorphine,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Active_Injection,0.700000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Post-Naltrexone,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "Post-Methadone,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "Post-Methadone,1_100,other,Active_Injection,0.700000"
            << std::endl
            << "Post-Methadone,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Post-Methadone,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "Post-Detox,1_100,other,Active_Noninjection,0.700000"
            << std::endl
            << "Post-Detox,1_100,other,Active_Injection,0.700000" << std::endl
            << "Post-Detox,1_100,other,Nonactive_Noninjection,0.700000"
            << std::endl
            << "Post-Detox,1_100,other,Nonactive_Injection,0.700000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "agegrp,race,sex,percent_overdoses_fatal_1_52" << std::endl
            << "1_100,other,0.700000" << std::endl
            << "";
        return stream.str();
    }

    std::string LoadWriteHistoryDataStream() const {
        std::stringstream stream;
        stream
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.900000,"
               "0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+1,t+52"
            << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.700000,0.600000,"
               "0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.700000,0.600000,"
               "0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.700000,0.600000,"
               "0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.700000,0.600000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.700000,"
               "0.600000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.700000,0.600000,"
               "0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.700000,0.600000,"
               "0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.700000,0.600000,0."
               "800000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+1,t+52"
            << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.400000,0.600000,"
               "0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.400000,0.600000,"
               "0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.400000,0.600000,"
               "0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.400000,0.600000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.400000,"
               "0.600000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.400000,0.600000,"
               "0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.400000,0.600000,"
               "0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.400000,0."
               "600000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.400000,0.600000,0."
               "800000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+1,t+52"
            << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.300000,0.200000,"
               "0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.300000,0.200000,"
               "0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.300000,0.200000,"
               "0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.300000,0.200000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.300000,"
               "0.200000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.300000,0.200000,"
               "0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.300000,0.200000,"
               "0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.300000,0.200000,0."
               "800000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+1,t+52"
            << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.100000,0.000000,"
               "0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.100000,0.000000,"
               "0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.100000,0.000000,"
               "0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.100000,0.000000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.100000,"
               "0.000000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.100000,0.000000,"
               "0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.100000,0.000000,"
               "0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.100000,0.000000,0."
               "800000"
            << std::endl
            << "";
        return stream.str();
    }

    std::string LoadWriteCostDataStream() const {
        std::stringstream stream;

        stream
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.900000,"
               "0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.300000,"
               "0.200000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.300000,0."
               "200000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.300000,0.200000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.300000,0.200000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.500000,"
               "0.400000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.500000,0."
               "400000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.500000,0.400000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.500000,0.400000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.700000,"
               "0.600000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.700000,0."
               "600000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.700000,0.600000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.700000,0.600000"
            << std::endl
            << "" << std::endl
            << "" << std::endl
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.100000,"
               "0.000000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.100000,0."
               "000000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.100000,0.000000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.100000,0.000000"
            << std::endl
            << "";

        return stream.str();
    }

    std::string LoadUtilityDataStream() const {
        std::stringstream stream;

        stream
            << "Interventions,OUD States,age_groups,sex,t+0,t+52" << std::endl
            << "No_Treatment,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "No_Treatment,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "No_Treatment,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "No_Treatment,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Buprenorphine,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Buprenorphine,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Naltrexone,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Methadone,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Detox,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Buprenorphine,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Active_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Noninjection,1_100,other,0.900000,"
               "0.800000"
            << std::endl
            << "Post-Buprenorphine,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Naltrexone,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Active_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Methadone,Nonactive_Noninjection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Methadone,Nonactive_Injection,1_100,other,0.900000,0."
               "800000"
            << std::endl
            << "Post-Detox,Active_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Active_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Noninjection,1_100,other,0.900000,0.800000"
            << std::endl
            << "Post-Detox,Nonactive_Injection,1_100,other,0.900000,0.800000"
            << std::endl
            << "";
        return stream.str();
    }

    std::string LoadTotalsDataStream() const {
        std::stringstream stream;
        stream << "Name,Base,Discount" << std::endl
               << "Perspective 0,100,90" << std::endl
               << "Perspective 1,200,180" << std::endl
               << "Perspective 2,300,270" << std::endl
               << "Life Years,100,90" << std::endl
               << "Utility,0.8,0.7" << std::endl
               << "";
        return stream.str();
    }

public:
    const std::string kWriterTestWriteInputDataExpected;
    const std::string kWriterTestWriteHistoryDataExpected;
    const std::string kWriterTestWriteCostDataExpected;
    const std::string kWriterTestWriteUtilityDataExpected;
    const std::string kWriterTestWriteTotalsDataExpected;

    ExpectedStrings()
        : kWriterTestWriteInputDataExpected(LoadWriteInputDataStream()),
          kWriterTestWriteHistoryDataExpected(LoadWriteHistoryDataStream()),
          kWriterTestWriteCostDataExpected(LoadWriteCostDataStream()),
          kWriterTestWriteUtilityDataExpected(LoadUtilityDataStream()),
          kWriterTestWriteTotalsDataExpected(LoadTotalsDataStream()) {}
};
} // namespace respond::tests
#endif