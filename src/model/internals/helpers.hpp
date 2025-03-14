////////////////////////////////////////////////////////////////////////////////
// File: helpers.hpp                                                          //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MODEL_HELPERS_HPP_
#define RESPOND_MODEL_HELPERS_HPP_

#include <iostream>
#include <string>
#include <vector>

#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>

namespace respond::model {
    bool argChecks(int argc, char **argv, std::string &rootInputDir,
                   int &taskStart, int &taskEnd) {
        if (argc > 1 && argc != 4) {
            std::cerr << "Usage: " << argv[0]
                      << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                      << "RESPOND, a compartmental simulation of healthcare in "
                         "communities with high-risk opioid use";
            return false;
        }

        if (argc == 1) {
            std::cout << "Please provide the input folder path: ";
            std::cin >> rootInputDir;
            std::cout << std::endl
                      << "Please provide the first input folder number: ";
            std::cin >> taskStart;
            std::cout << std::endl
                      << "Please provide the last input folder number: ";
            std::cin >> taskEnd;
        } else {
            taskStart = std::stoi(argv[2]);
            taskEnd = std::stoi(argv[3]);
            rootInputDir = argv[1];
        }
        return true;
    }

    std::vector<double>
    CalculateTotalCosts(const respond::data_ops::CostList &cost_list) {
        std::vector<double> result;
        for (respond::data_ops::Cost cost : cost_list) {
            double totalCost =
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.healthcare_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.fatal_overdose_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.non_fatal_overdose_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.pharma_cost) +
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    cost.treatment_cost);
            result.push_back(totalCost);
        }
        return result;
    }
} // namespace respond::model

#endif // RESPOND_MODEL_HELPERS_HPP_
