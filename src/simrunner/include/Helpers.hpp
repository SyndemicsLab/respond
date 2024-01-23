#ifndef UTILITIES_HELPERS_HPP_
#define UTILITIES_HELPERS_HPP_

#include <iostream>
#include <string>
#include <vector>

#include "DataLoader.hpp"

namespace Helpers {
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

    std::vector<double> calcCosts(Calculator::PostSimulationCalculator calc,
                                  Matrixify::CostList costsList) {
        std::vector<double> result;
        for (Matrixify::Cost cost : costsList) {
            double totalCost = 0.0;
            totalCost += calc.totalAcrossTimeAndDims(cost.healthcareCost);
            totalCost += calc.totalAcrossTimeAndDims(cost.fatalOverdoseCost);
            totalCost += calc.totalAcrossTimeAndDims(cost.nonFatalOverdoseCost);
            totalCost += calc.totalAcrossTimeAndDims(cost.pharmaCost);
            totalCost += calc.totalAcrossTimeAndDims(cost.treatmentCost);
            result.push_back(totalCost);
        }
        return result;
    }
} // namespace Helpers

#endif