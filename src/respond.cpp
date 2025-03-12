////////////////////////////////////////////////////////////////////////////////
// File: respond.cpp                                                //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/Writer.hpp>
#include <respond/model/Helpers.hpp>
#include <respond/model/PostSimulationCalculator.hpp>
#include <respond/model/Simulation.hpp>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cassert>
#include <execution>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <respond/data_ops/CostLoader.hpp>
#include <respond/data_ops/DataFormatter.hpp>
#include <respond/data_ops/DataLoader.hpp>
#include <respond/model/Helpers.hpp>
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

            std::shared_ptr<data_ops::IDataLoader> inputs =
                std::make_shared<data_ops::DataLoader>(
                    nullptr, inputSet.string(), logger);
            logger->info("DataLoader Created");

            std::shared_ptr<data_ops::ICostLoader> costLoader =
                std::make_shared<data_ops::CostLoader>(inputSet.string());
            logger->info("CostLoader Created");

            std::shared_ptr<data_ops::IUtilityLoader> utilityLoader =
                std::make_shared<data_ops::UtilityLoader>(inputSet.string());
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

            Simulation::Respond sim(inputs);
            sim.run();
            data_ops::History history = sim.getHistory();

            data_ops::CostList basecosts;
            data_ops::TimedMatrix3d baseutilities;
            double baselifeYears = 0.0;
            std::vector<double> totalBaseCosts;
            double totalBaseUtility = 0.0;

            data_ops::CostList disccosts;
            data_ops::TimedMatrix3d discutilities;
            double disclifeYears;
            std::vector<double> totalDiscCosts;
            double totalDiscUtility = 0.0;

            if (costLoader->getCostSwitch()) {
                Calculator::PostSimulationCalculator PostSimulationCalculator(
                    history);
                basecosts = PostSimulationCalculator.calculateCosts(costLoader);
                totalBaseCosts =
                    Helpers::calcCosts(PostSimulationCalculator, basecosts);

                baseutilities = PostSimulationCalculator.calculateUtilities(
                    utilityLoader, Calculator::UTILITY_TYPE::MIN);
                totalBaseUtility =
                    PostSimulationCalculator.totalAcrossTimeAndDims(
                        baseutilities);
                baselifeYears = PostSimulationCalculator.calculateLifeYears();
                if (costLoader->getDiscountRate() != 0.0) {
                    disccosts =
                        PostSimulationCalculator.calculateCosts(costLoader);

                    totalDiscCosts =
                        Helpers::calcCosts(PostSimulationCalculator, disccosts);
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
                inputs->getGeneralStatsOutputTimesteps();

            bool pivot_long = false;
            pivot_long = std::get<bool>(
                inputs->getConfig()->get("output.pivot_long", pivot_long));

            data_ops::HistoryWriter historyWriter(
                outputDir.string(), inputs->getInterventions(),
                inputs->getOUDStates(), inputs->getDemographics(),
                inputs->getDemographicCombos(), outputTimesteps,
                data_ops::WriteType::FILE, pivot_long);

            data_ops::DataFormatter formatter;

            formatter.extractTimesteps(outputTimesteps, history, basecosts,
                                       baseutilities,
                                       costLoader->getCostSwitch());

            historyWriter.writeHistory(history);

            bool writeParameters = false;
            writeParameters = std::get<bool>(inputs->getConfig()->get(
                "output.write_calibrated_inputs", writeParameters));
            if (writeParameters) {
                data_ops::InputWriter ipWriter(outputDir.string(),
                                               outputTimesteps,
                                               data_ops::WriteType::FILE);
                ipWriter.writeParameters(inputs);
            }

            // Probably want to figure out the right way to do this
            if (costLoader->getCostSwitch()) {
                data_ops::CostWriter costWriter(
                    outputDir.string(), inputs->getInterventions(),
                    inputs->getOUDStates(), inputs->getDemographics(),
                    inputs->getDemographicCombos(), outputTimesteps,
                    data_ops::WriteType::FILE, pivot_long);
                costWriter.writeCosts(basecosts);
            }
            if (utilityLoader->getCostSwitch()) {
                data_ops::UtilityWriter utilityWriter(
                    outputDir.string(), inputs->getInterventions(),
                    inputs->getOUDStates(), inputs->getDemographics(),
                    inputs->getDemographicCombos(), outputTimesteps,
                    data_ops::WriteType::FILE, pivot_long);
                utilityWriter.writeUtilities(baseutilities);
            }
            if (costLoader->getCostSwitch()) {
                data_ops::Totals totals;
                totals.baseCosts = totalBaseCosts;
                totals.baseLifeYears = baselifeYears;
                totals.baseUtility = totalBaseUtility;
                totals.discCosts = totalDiscCosts;
                totals.discLifeYears = disclifeYears;
                totals.discUtility = totalDiscUtility;
                data_ops::TotalsWriter totalsWriter(
                    outputDir.string(), inputs->getInterventions(),
                    inputs->getOUDStates(), inputs->getDemographics(),
                    inputs->getDemographicCombos(), outputTimesteps,
                    data_ops::WriteType::FILE);
                totalsWriter.writeTotals(totals);
            }

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "Simulation Complete! :)" << std::endl;
    return 0;
}
