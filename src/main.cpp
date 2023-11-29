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

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include "CostLoader.hpp"
#include "DataFormatter.hpp"
#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "PostSimulationCalculator.hpp"
#include "Simulation.hpp"

bool argChecks(int argc, char **argv, std::string &rootInputDir, int &taskStart,
               int &taskEnd) {
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
                              Data::CostList costsList) {
    std::vector<double> result;
    for (Data::Cost cost : costsList) {
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

int main(int argc, char **argv) {

    int taskStart;
    int taskEnd;
    std::string rootInputDir;
    if (!argChecks(argc, argv, rootInputDir, taskStart, taskEnd)) {
        return 0;
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
            std::filesystem::create_directory(outputDir);

            std::string log_path = outputDir.string() + "/log.txt";
            std::shared_ptr<spdlog::logger> logger;

            try {
                logger = spdlog::basic_logger_mt("logger" + std::to_string(i),
                                                 log_path);
            } catch (const spdlog::spdlog_ex &ex) {
                std::cout << "Log init failed: " << ex.what() << std::endl;
                return;
            }

            logger->info("Logger Created");

            Data::DataLoader inputs(inputSet.string(), logger);
            logger->info("DataLoader Created");

            Data::CostLoader costLoader(inputSet.string());
            logger->info("CostLoader Created");

            Data::UtilityLoader utilityLoader(inputSet.string());
            logger->info("UtilityLoader Created");

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

            Data::CostList basecosts;
            Data::Matrix3dOverTime baseutilities;
            double baselifeYears = 0.0;
            std::vector<double> totalBaseCosts;
            double totalBaseUtility = 0.0;

            Data::CostList disccosts;
            Data::Matrix3dOverTime discutilities;
            double disclifeYears;
            std::vector<double> totalDiscCosts;
            double totalDiscUtility = 0.0;

            if (costLoader.getCostSwitch()) {
                Calculator::PostSimulationCalculator PostSimulationCalculator(
                    history);
                basecosts = PostSimulationCalculator.calculateCosts(costLoader);
                totalBaseCosts = calcCosts(PostSimulationCalculator, basecosts);

                baseutilities = PostSimulationCalculator.calculateUtilities(
                    utilityLoader, Calculator::UTILITY_TYPE::MIN);
                totalBaseUtility =
                    PostSimulationCalculator.totalAcrossTimeAndDims(
                        baseutilities);
                baselifeYears = PostSimulationCalculator.calculateLifeYears();
                if (costLoader.getDiscountRate() != 0.0) {
                    disccosts =
                        PostSimulationCalculator.calculateCosts(costLoader);

                    totalDiscCosts =
                        calcCosts(PostSimulationCalculator, disccosts);
                    discutilities = PostSimulationCalculator.calculateUtilities(
                        utilityLoader, Calculator::UTILITY_TYPE::MIN);
                    totalDiscUtility =
                        PostSimulationCalculator.totalAcrossTimeAndDims(
                            discutilities);
                    disclifeYears =
                        PostSimulationCalculator.calculateLifeYears();
                }
            }

            std::vector<int> outputTimesteps =
                inputs.getGeneralStatsOutputTimesteps();

            Data::DataWriter writer(
                outputDir.string(), inputs.getInterventions(),
                inputs.getOUDStates(), demographics, outputTimesteps,
                inputs.getGeneralOutputsSwitch());

            Data::DataFormatter formatter;

            formatter.extractTimesteps(outputTimesteps, history, basecosts,
                                       baseutilities,
                                       costLoader.getCostSwitch());

            writer.writeHistory(Data::FILE, history);

            Data::Totals totals;
            totals.baseCosts = totalBaseCosts;
            totals.baseLifeYears = baselifeYears;
            totals.baseUtility = totalBaseUtility;
            totals.discCosts = totalDiscCosts;
            totals.discLifeYears = disclifeYears;
            totals.discUtility = totalDiscUtility;

            if (costLoader.getCostSwitch()) {
                writer.writeCosts(Data::FILE, basecosts);
                writer.writeUtilities(Data::FILE, baseutilities);
                writer.writeTotals(Data::FILE, totals);
            }

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "Simulation Complete! :)" << std::endl;
    return 0;
}
