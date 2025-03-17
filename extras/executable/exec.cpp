////////////////////////////////////////////////////////////////////////////////
// File: exec.cpp                                                             //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <execution>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <respond/respond.hpp>

bool ArgChecks(int argc, char **argv, std::string &root, int &start, int &end) {
    if (argc > 1 && argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                  << "RESPOND, a compartmental simulation of healthcare in "
                     "communities with high-risk opioid use";
        return false;
    }

    if (argc == 1) {
        std::cout << "Please provide the input folder path: ";
        std::cin >> root;
        std::cout << std::endl
                  << "Please provide the first input folder number: ";
        std::cin >> start;
        std::cout << std::endl
                  << "Please provide the last input folder number: ";
        std::cin >> end;
    } else {
        start = std::stoi(argv[2]);
        end = std::stoi(argv[3]);
        root = argv[1];
    }
    return true;
}

void LoadDataLoader(respond::data_ops::DataLoader &data_loader) {
    data_loader.LoadInitialSample("init_cohort.csv");
    if (std::get<bool>(data_loader.GetConfig()->get(
            "simulation.stratified_entering_cohort", false))) {
        data_loader.LoadEnteringSamples("entering_cohort.csv");
    } else {
        data_loader.LoadEnteringSamples("entering_cohort.csv", "No_Treatment",
                                        "Active_Noninjection");
    }
    data_loader.LoadOUDTransitionRates("oud_trans.csv");
    data_loader.LoadInterventionInitRates("block_init_effect.csv");
    data_loader.LoadInterventionTransitionRates("block_trans.csv");
    data_loader.LoadOverdoseRates("all_types_overdose.csv");
    data_loader.LoadFatalOverdoseRates("fatal_overdose.csv");
    data_loader.LoadMortalityRates("SMR.csv", "background_mortality.csv");
}

int main(int argc, char **argv) {
    int start;
    int end;
    std::string root;
    if (!ArgChecks(argc, argv, root, start, end)) {
        return 0;
    }

    std::vector<int> runs((end + 1) - start);
    std::iota(std::begin(runs), std::end(runs), start);

    std::for_each(
        std::execution::par_unseq, std::begin(runs), std::end(runs),
        [&](int i) {
            std::filesystem::path input_directory = root;
            std::filesystem::path input_set =
                input_directory / ("input" + std::to_string(i));
            std::filesystem::path output_directory =
                input_directory / ("output" + std::to_string(i));
            std::filesystem::create_directory(output_directory);

            std::string log_path = output_directory.string() + "/log.txt";
            std::string log_name = "logger" + std::to_string(i);
            respond::utils::CreateFileLogger(log_name, log_path);

            respond::utils::LogInfo(log_name, "Logger Created");

            auto data_loader = respond::data_ops::DataLoader::Create(
                input_set.string(), log_name);
            respond::utils::LogInfo(log_name, "DataLoader Created");

            LoadDataLoader(*data_loader);

            auto respond = respond::model::Respond::Create(log_name);
            respond->Run(*data_loader);
            respond::data_ops::History history = respond->GetHistory();

            auto costLoader = respond::data_ops::CostLoader::Create(
                input_set.string(), log_name);
            respond::utils::LogInfo(log_name, "CostLoader Created");

            auto utilityLoader = respond::data_ops::UtilityLoader::Create(
                input_set.string(), log_name);
            respond::utils::LogInfo(log_name, "UtilityLoader Created");

            if (std::get<bool>(data_loader->GetConfig()->get(
                    "cost.cost_analysis", false))) {
                costLoader->LoadHealthcareUtilizationCost(
                    "healthcare_utilization_cost.csv");
                costLoader->LoadOverdoseCost("overdose_cost.csv");
                costLoader->LoadPharmaceuticalCost("pharmaceutical_cost.csv");
                costLoader->LoadTreatmentUtilizationCost(
                    "treatment_utilization_cost.csv");

                utilityLoader->LoadBackgroundUtility("bg_utility.csv");
                utilityLoader->LoadOUDUtility("oud_utility.csv");
                utilityLoader->LoadSettingUtility("setting_utility.csv");

                basecosts = respond::model::CalculateCosts(
                    history, *costLoader,
                    data_loader->GetConfig()->getStringVector(
                        "cost.cost_perspectives"));

                totalBaseCosts =
                    Helpers::calcCosts(PostSimulationCalculator, basecosts);

                baseutilities = PostSimulationCalculator.calculateUtilities(
                    utilityLoader, Calculator::UTILITY_TYPE::MIN);
                totalBaseUtility =
                    PostSimulationCalculator.totalAcrossTimeAndDims(
                        baseutilities);
                baselifeYears = PostSimulationCalculator.calculateLifeYears();
                if (costLoader->GetDiscountRate() != 0.0) {
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

            respond::data_ops::CostList basecosts;
            respond::data_ops::TimedMatrix3d baseutilities;
            double baselifeYears = 0.0;
            std::vector<double> totalBaseCosts;
            double totalBaseUtility = 0.0;

            respond::data_ops::CostList disccosts;
            respond::data_ops::TimedMatrix3d discutilities;
            double disclifeYears;
            std::vector<double> totalDiscCosts;
            double totalDiscUtility = 0.0;

            std::vector<int> outputTimesteps =
                data_loader->GetGeneralStatsOutputTimesteps();

            bool pivot_long = false;
            pivot_long = std::get<bool>(
                data_loader->GetConfig()->Get("output.pivot_long", pivot_long));

            data_ops::HistoryWriter historyWriter(
                output_directory.string(), data_loader->GetInterventions(),
                data_loader->GetOUDStates(), data_loader->GetDemographics(),
                data_loader->GetDemographicCombos(), outputTimesteps,
                data_ops::WriteType::FILE, pivot_long);

            data_ops::DataFormatter formatter;

            formatter.extractTimesteps(outputTimesteps, history, basecosts,
                                       baseutilities,
                                       costLoader->GetCostSwitch());

            historyWriter.writeHistory(history);

            bool writeParameters = false;
            writeParameters = std::get<bool>(data_loader->GetConfig()->Get(
                "output.write_calibrated_inputs", writeParameters));
            if (writeParameters) {
                data_ops::InputWriter ipWriter(output_directory.string(),
                                               outputTimesteps,
                                               data_ops::WriteType::FILE);
                ipWriter.writeParameters(data_loader);
            }

            // Probably want to figure out the right way to do this
            if (costLoader->GetCostSwitch()) {
                data_ops::CostWriter costWriter(
                    output_directory.string(), data_loader->GetInterventions(),
                    data_loader->GetOUDStates(), data_loader->GetDemographics(),
                    data_loader->GetDemographicCombos(), outputTimesteps,
                    data_ops::WriteType::FILE, pivot_long);
                costWriter.writeCosts(basecosts);
            }
            if (utilityLoader->GetCostSwitch()) {
                data_ops::UtilityWriter utilityWriter(
                    output_directory.string(), data_loader->GetInterventions(),
                    data_loader->GetOUDStates(), data_loader->GetDemographics(),
                    data_loader->GetDemographicCombos(), outputTimesteps,
                    data_ops::WriteType::FILE, pivot_long);
                utilityWriter.writeUtilities(baseutilities);
            }
            if (costLoader->GetCostSwitch()) {
                data_ops::Totals totals;
                totals.baseCosts = totalBaseCosts;
                totals.baseLifeYears = baselifeYears;
                totals.baseUtility = totalBaseUtility;
                totals.discCosts = totalDiscCosts;
                totals.discLifeYears = disclifeYears;
                totals.discUtility = totalDiscUtility;
                data_ops::TotalsWriter totalsWriter(
                    output_directory.string(), data_loader->GetInterventions(),
                    data_loader->GetOUDStates(), data_loader->GetDemographics(),
                    data_loader->GetDemographicCombos(), outputTimesteps,
                    data_ops::WriteType::FILE);
                totalsWriter.writeTotals(totals);
            }

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "Simulation Complete! :)" << std::endl;
    return 0;
}