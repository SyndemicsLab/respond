//===-- main.cpp - Instruction class definition -----------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <cassert>
#include <execution>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <vector>

#include "CostLoader.hpp"
#include "DataFormatter.hpp"
#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "PostSimulationCalculator.hpp"
#include "Simulation.hpp"

int main(int argc, char **argv) {
    if (argc > 1 && argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                  << "RESPOND, a compartmental simulation of healthcare in "
                     "communities with high-risk opioid use";
    }
    int taskStart;
    int taskEnd;
    std::string rootInputDir;
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

    std::vector<int> runs((taskEnd + 1) - taskStart);
    std::iota(std::begin(runs), std::end(runs), taskStart);

    std::for_each(
        std::execution::par_unseq, std::begin(runs), std::end(runs),
        [&](int i) {
            std::filesystem::path inputDir = rootInputDir;
            std::filesystem::path inputSet =
                inputDir / ("input" + std::to_string(i));
            std::filesystem::path outputDir =
                inputDir / ("output" + std::to_string(i));

            Data::DataLoader inputs(inputSet.string());
            Data::CostLoader costLoader(inputSet.string());
            Data::UtilityLoader utilityLoader(inputSet.string());

            Data::Costs costs;
            Data::Utility util;

            inputs.loadInitialSample("init_cohort.csv");
            inputs.loadEnteringSamples("entering_cohort.csv", "No_Treatment",
                                       "Active_Noninjection");
            inputs.loadOUDTransitionRates("oud_trans.csv");
            inputs.loadInterventionInitRates("block_init_effect.csv");
            inputs.loadInterventionTransitionRates("block_trans.csv");
            inputs.loadOverdoseRates("all_types_overdose.csv");
            inputs.loadFatalOverdoseRates("fatal_overdose.csv");
            inputs.loadMortalityRates("SMR.csv", "background_mortality.csv");

            if (costLoader.getCostSwitch()) {
                costLoader.loadHealthcareUtilizationCost(
                    "healthcare_utilization_cost.csv");
                costLoader.loadOverdoseCost("overdose_cost.csv");
                costLoader.loadPharmaceuticalCost("pharmaceutical_cost.csv");
                costLoader.loadTreatmentUtilizationCost(
                    "treatment_utilization_cost.csv");

                utilityLoader.loadBackgroundUtility("bg_utility.csv");
                utilityLoader.loadOUDUtility("oud_utility.csv");
                utilityLoader.loadSettingUtility("setting_utility.csv");
            }

            std::vector<std::vector<std::string>> demographics =
                inputs.getConfiguration().getDemographicCombosVecOfVec();

            Simulation::Sim sim(inputs);
            sim.Run();
            Data::History history = sim.getHistory();

            if (costLoader.getCostSwitch()) {
                Calculator::PostSimulationCalculator PostSimulationCalculator(
                    history);
                costs = PostSimulationCalculator.calculateCosts(costLoader);
                util = PostSimulationCalculator.calculateUtility(utilityLoader);
            }

            std::vector<int> outputTimesteps =
                inputs.getGeneralStatsOutputTimesteps();

            Data::DataWriter writer(
                outputDir.string(), inputs.getInterventions(),
                inputs.getOUDStates(), demographics, outputTimesteps,
                inputs.getGeneralOutputsSwitch());

            Data::DataFormatter formatter;

            formatter.extractTimesteps(outputTimesteps, history, costs, util,
                                       costLoader.getCostSwitch());

            writer.writeHistory(Data::FILE, history);

            if (costLoader.getCostSwitch()) {
                writer.writeCosts(Data::FILE, costs);
                writer.writeUtility(Data::FILE, util);
            }

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "Simulation Complete! :)" << std::endl;
    return 0;
}
