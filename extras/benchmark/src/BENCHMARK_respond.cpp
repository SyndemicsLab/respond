#include "CostLoader.hpp"
#include "DataFormatter.hpp"
#include "DataLoader.hpp"
#include "Helpers.hpp"
#include "PostSimulationCalculator.hpp"
#include "Simulation.hpp"
#include "Writer.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <cassert>
#include <execution>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <vector>

std::filesystem::path BENCHMARK_INPUT =
    std::filesystem::temp_directory_path() / "benchmark";

static int respond_main(const int inputID) {
    std::filesystem::path inputSet =
        BENCHMARK_INPUT / ("input" + std::to_string(inputID));
    std::filesystem::path outputDir =
        BENCHMARK_INPUT / ("output" + std::to_string(inputID));
    std::filesystem::create_directory(outputDir);

    std::string log_path = outputDir.string() + "/log.txt";
    std::shared_ptr<spdlog::logger> logger;

    try {
        logger = spdlog::basic_logger_mt("logger" + std::to_string(inputID),
                                         log_path);
#ifndef NDEBUG
        spdlog::set_level(spdlog::level::debug);
#endif
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }

    logger->info("Logger Created");

    std::shared_ptr<Matrixify::IDataLoader> inputs =
        std::make_shared<Matrixify::DataLoader>(nullptr, inputSet.string(),
                                                logger);
    logger->info("DataLoader Created");

    std::shared_ptr<Matrixify::ICostLoader> costLoader =
        std::make_shared<Matrixify::CostLoader>(inputSet.string());
    logger->info("CostLoader Created");

    std::shared_ptr<Matrixify::IUtilityLoader> utilityLoader =
        std::make_shared<Matrixify::UtilityLoader>(inputSet.string());
    logger->info("UtilityLoader Created");

    inputs->loadInitialSample("init_cohort.csv");
    if (inputs->getEnteringCohortToggle()) {
        inputs->loadEnteringSamples("entering_cohort.csv");
    } else {
        inputs->loadEnteringSamples("entering_cohort.csv", "No_Treatment",
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
    Matrixify::History history = sim.getHistory();

    Matrixify::CostList basecosts;
    Matrixify::Matrix4d baseutilities;
    double baselifeYears = 0.0;
    std::vector<double> totalBaseCosts;
    double totalBaseUtility = 0.0;

    Matrixify::CostList disccosts;
    Matrixify::Matrix4d discutilities;
    double disclifeYears;
    std::vector<double> totalDiscCosts;
    double totalDiscUtility = 0.0;

    if (costLoader->getCostSwitch()) {
        Calculator::PostSimulationCalculator PostSimulationCalculator(history);
        basecosts = PostSimulationCalculator.calculateCosts(costLoader);
        totalBaseCosts =
            Helpers::calcCosts(PostSimulationCalculator, basecosts);

        baseutilities = PostSimulationCalculator.calculateUtilities(
            utilityLoader, Calculator::UTILITY_TYPE::MIN);
        totalBaseUtility =
            PostSimulationCalculator.totalAcrossTimeAndDims(baseutilities);
        baselifeYears = PostSimulationCalculator.calculateLifeYears();
        if (costLoader->getDiscountRate() != 0.0) {
            disccosts = PostSimulationCalculator.calculateCosts(costLoader);

            totalDiscCosts =
                Helpers::calcCosts(PostSimulationCalculator, disccosts);
            discutilities = PostSimulationCalculator.calculateUtilities(
                utilityLoader, Calculator::UTILITY_TYPE::MIN);
            totalDiscUtility =
                PostSimulationCalculator.totalAcrossTimeAndDims(discutilities);
            disclifeYears = PostSimulationCalculator.calculateLifeYears();
        }
    }

    std::vector<int> outputTimesteps = inputs->getGeneralStatsOutputTimesteps();

    bool pivot_long = false;
    pivot_long = std::get<bool>(
        inputs->getConfig()->get("output.pivot_long", pivot_long));

    Matrixify::HistoryWriter historyWriter(
        outputDir.string(), inputs->getInterventions(), inputs->getOUDStates(),
        inputs->getDemographics(), inputs->getDemographicCombos(),
        outputTimesteps, Matrixify::WriteType::FILE, pivot_long);

    Matrixify::DataFormatter formatter;

    formatter.extractTimesteps(outputTimesteps, history, basecosts,
                               baseutilities, costLoader->getCostSwitch());

    historyWriter.writeHistory(history);

    bool writeParameters = false;
    writeParameters = std::get<bool>(inputs->getConfig()->get(
        "output.write_calibrated_inputs", writeParameters));
    if (writeParameters) {
        Matrixify::InputWriter ipWriter(outputDir.string(), outputTimesteps,
                                        Matrixify::WriteType::FILE);
        ipWriter.writeParameters(inputs);
    }

    // Probably want to figure out the right way to do this
    if (costLoader->getCostSwitch()) {
        Matrixify::CostWriter costWriter(
            outputDir.string(), inputs->getInterventions(),
            inputs->getOUDStates(), inputs->getDemographics(),
            inputs->getDemographicCombos(), outputTimesteps,
            Matrixify::WriteType::FILE, pivot_long);
        costWriter.writeCosts(basecosts);
    }
    if (utilityLoader->getCostSwitch()) {
        Matrixify::UtilityWriter utilityWriter(
            outputDir.string(), inputs->getInterventions(),
            inputs->getOUDStates(), inputs->getDemographics(),
            inputs->getDemographicCombos(), outputTimesteps,
            Matrixify::WriteType::FILE, pivot_long);
        utilityWriter.writeUtilities(baseutilities);
    }
    if (costLoader->getCostSwitch()) {
        Matrixify::Totals totals;
        totals.baseCosts = totalBaseCosts;
        totals.baseLifeYears = baselifeYears;
        totals.baseUtility = totalBaseUtility;
        totals.discCosts = totalDiscCosts;
        totals.discLifeYears = disclifeYears;
        totals.discUtility = totalDiscUtility;
        Matrixify::TotalsWriter totalsWriter(
            outputDir.string(), inputs->getInterventions(),
            inputs->getOUDStates(), inputs->getDemographics(),
            inputs->getDemographicCombos(), outputTimesteps,
            Matrixify::WriteType::FILE);
        totalsWriter.writeTotals(totals);
    }

    std::cout << "Output " << std::to_string(inputID) << " Complete"
              << std::endl;

    std::cout << "Simulation Complete! :)" << std::endl;
    return 0;
}

static void run(benchmark::State &state) { respond_main(1); }

// mark functions for benchmarking
BENCHMARK(run);

// run benchmarks
BENCHMARK_MAIN();
