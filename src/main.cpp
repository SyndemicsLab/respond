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
/// This file contains the primary main function to start the RESPOND
/// simulation model. It expects 3 command line arguments:
///     (std::string)root_input_directory (int)task_start (int)task_end
/// After verifying args it creates a parallel loop to run tasks from
/// task_start until reaching and including task_end.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "CostLoader.hpp"
#include "DataFormatter.hpp"
#include "DataLoader.hpp"
#include "Helpers.hpp"
#include "PostsimulationCalculator.hpp"
#include "Writer.hpp"
#include "simulation.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cassert>
#include <execution>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <vector>

int main(int argc, char **argv) {
    int taskStart;
    int taskEnd;
    std::string rootInputDir;
    if (!Helpers::argChecks(argc, argv, rootInputDir, taskStart, taskEnd)) {
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
#ifndef NDEBUG
                spdlog::set_level(spdlog::level::debug);
#endif
            } catch (const spdlog::spdlog_ex &ex) {
                std::cout << "Log init failed: " << ex.what() << std::endl;
                return;
            }

            logger->info("Logger Created");

            std::shared_ptr<matrixify::IDataLoader> inputs =
                std::make_shared<matrixify::DataLoader>(
                    nullptr, inputSet.string(), logger);
            logger->info("DataLoader Created");

            std::shared_ptr<matrixify::ICostLoader> costLoader =
                std::make_shared<matrixify::CostLoader>(inputSet.string());
            logger->info("CostLoader Created");

            std::shared_ptr<matrixify::IUtilityLoader> utilityLoader =
                std::make_shared<matrixify::UtilityLoader>(inputSet.string());
            logger->info("UtilityLoader Created");

            inputs->loadInitialSample("init_cohort.csv");
            if (inputs->getEnteringCohortToggle()) {
                inputs->loadEnteringSamples("entering_cohort.csv");
            } else {
                inputs->loadEnteringSamples("entering_cohort.csv",
                                            "No_Treatment",
                                            "Active_Noninjection");
            }
            inputs->loadOUDTransitionRates("oud_trans.csv");
            inputs->loadInterventionInitRates("block_init_effect.csv");
            inputs->loadInterventionTransitionRates("block_trans.csv");
            inputs->loadOverdoseRates("all_types_overdose.csv");
            inputs->loadFatalOverdoseRates("fatal_overdose.csv");
            inputs->loadMortalityRates("SMR.csv", "background_mortality.csv");

            if (costLoader->getCostSwitch()) {
                costLoader->loadHealthcareUtilizationCost(
                    "healthcare_utilization_cost.csv");
                costLoader->loadOverdoseCost("overdose_cost.csv");
                costLoader->loadPharmaceuticalCost("pharmaceutical_cost.csv");
                costLoader->loadTreatmentUtilizationCost(
                    "treatment_utilization_cost.csv");

                utilityLoader->loadBackgroundUtility("bg_utility.csv");
                utilityLoader->loadOUDUtility("oud_utility.csv");
                utilityLoader->loadSettingUtility("setting_utility.csv");
            }

            simulation::Respond sim(inputs);
            sim.run();
            matrixify::History history = sim.getHistory();

            matrixify::CostList basecosts;
            matrixify::Matrix4d baseutilities;
            double baselifeYears = 0.0;
            std::vector<double> totalBaseCosts;
            double totalBaseUtility = 0.0;

            matrixify::CostList disccosts;
            matrixify::Matrix4d discutilities;
            double disclifeYears;
            std::vector<double> totalDiscCosts;
            double totalDiscUtility = 0.0;

            if (costLoader->getCostSwitch()) {
                Calculator::PostsimulationCalculator PostsimulationCalculator(
                    history);
                basecosts = PostsimulationCalculator.calculateCosts(costLoader);
                totalBaseCosts =
                    Helpers::calcCosts(PostsimulationCalculator, basecosts);

                baseutilities = PostsimulationCalculator.calculateUtilities(
                    utilityLoader, Calculator::UTILITY_TYPE::MIN);
                totalBaseUtility =
                    PostsimulationCalculator.totalAcrossTimeAndDims(
                        baseutilities);
                baselifeYears = PostsimulationCalculator.calculateLifeYears();
                if (costLoader->getDiscountRate() != 0.0) {
                    disccosts =
                        PostsimulationCalculator.calculateCosts(costLoader);

                    totalDiscCosts =
                        Helpers::calcCosts(PostsimulationCalculator, disccosts);
                    discutilities = PostsimulationCalculator.calculateUtilities(
                        utilityLoader, Calculator::UTILITY_TYPE::MIN);
                    totalDiscUtility =
                        PostsimulationCalculator.totalAcrossTimeAndDims(
                            discutilities);
                    disclifeYears =
                        PostsimulationCalculator.calculateLifeYears();
                }
            }

            std::vector<int> outputTimesteps =
                inputs->getGeneralStatsOutputTimesteps();

            bool pivot_long = false;
            pivot_long = std::get<bool>(
                inputs->getConfig()->get("output.pivot_long", pivot_long));

            matrixify::HistoryWriter historyWriter(
                outputDir.string(), inputs->getInterventions(),
                inputs->getOUDStates(), inputs->getDemographics(),
                inputs->getDemographicCombos(), outputTimesteps,
                matrixify::WriteType::FILE, pivot_long);

            matrixify::DataFormatter formatter;

            formatter.extractTimesteps(outputTimesteps, history, basecosts,
                                       baseutilities,
                                       costLoader->getCostSwitch());

            historyWriter.writeHistory(history);

            bool writeParameters = false;
            writeParameters = std::get<bool>(inputs->getConfig()->get(
                "output.write_calibrated_inputs", writeParameters));
            if (writeParameters) {
                matrixify::InputWriter ipWriter(outputDir.string(),
                                                outputTimesteps,
                                                matrixify::WriteType::FILE);
                ipWriter.writeParameters(inputs);
            }

            // Probably want to figure out the right way to do this
            if (costLoader->getCostSwitch()) {
                matrixify::CostWriter costWriter(
                    outputDir.string(), inputs->getInterventions(),
                    inputs->getOUDStates(), inputs->getDemographics(),
                    inputs->getDemographicCombos(), outputTimesteps,
                    matrixify::WriteType::FILE, pivot_long);
                costWriter.writeCosts(basecosts);
            }
            if (utilityLoader->getCostSwitch()) {
                matrixify::UtilityWriter utilityWriter(
                    outputDir.string(), inputs->getInterventions(),
                    inputs->getOUDStates(), inputs->getDemographics(),
                    inputs->getDemographicCombos(), outputTimesteps,
                    matrixify::WriteType::FILE, pivot_long);
                utilityWriter.writeUtilities(baseutilities);
            }
            if (costLoader->getCostSwitch()) {
                matrixify::Totals totals;
                totals.baseCosts = totalBaseCosts;
                totals.baseLifeYears = baselifeYears;
                totals.baseUtility = totalBaseUtility;
                totals.discCosts = totalDiscCosts;
                totals.discLifeYears = disclifeYears;
                totals.discUtility = totalDiscUtility;
                matrixify::TotalsWriter totalsWriter(
                    outputDir.string(), inputs->getInterventions(),
                    inputs->getOUDStates(), inputs->getDemographics(),
                    inputs->getDemographicCombos(), outputTimesteps,
                    matrixify::WriteType::FILE);
                totalsWriter.writeTotals(totals);
            }

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "simulation Complete! :)" << std::endl;
    return 0;
}
