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

void LoadCostLoader(respond::data_ops::CostLoader &cost_loader) {
    cost_loader.LoadHealthcareUtilizationCost(
        "healthcare_utilization_cost.csv");
    cost_loader.LoadOverdoseCost("overdose_cost.csv");
    cost_loader.LoadPharmaceuticalCost("pharmaceutical_cost.csv");
    cost_loader.LoadTreatmentUtilizationCost("treatment_utilization_cost.csv");
}

void LoadUtilityLoader(respond::data_ops::UtilityLoader &utility_loader) {
    utility_loader.LoadBackgroundUtility("bg_utility.csv");
    utility_loader.LoadOUDUtility("oud_utility.csv");
    utility_loader.LoadSettingUtility("setting_utility.csv");
}

void DoPostSimulationCalculations(
    const respond::data_ops::DataLoader &data_loader,
    respond::data_ops::History history, const std::string &input_set,
    const std::string &output_directory, const std::string &logger_name) {
    if (!std::get<bool>(
            data_loader.GetConfig()->get("cost.cost_analysis", false))) {
        return;
    }
    auto cost_loader =
        respond::data_ops::CostLoader::Create(input_set, logger_name);
    respond::utils::LogInfo(logger_name, "CostLoader Created");

    LoadCostLoader(*cost_loader);

    auto utility_loader =
        respond::data_ops::UtilityLoader::Create(input_set, logger_name);
    respond::utils::LogInfo(logger_name, "UtilityLoader Created");

    LoadUtilityLoader(*utility_loader);

    respond::data_ops::CostList base_costs;
    respond::data_ops::TimedMatrix3d base_utilities;
    respond::data_ops::Totals totals;

    if (std::get<bool>(
            data_loader.GetConfig()->get("cost.cost_analysis", false))) {

        double disc_life_years = 0.0;
        std::vector<double> total_disc_costs = {};
        double total_disc_utility = 0.0;

        base_costs = respond::model::CalculateCosts(
            history, *cost_loader,
            data_loader.GetConfig()->getStringVector("cost.cost_perspectives"));

        base_utilities = respond::model::CalculateUtilities(
            history, *utility_loader, respond::data_ops::UtilityType::kMin);

        double discount_rate = std::get<double>(
            data_loader.GetConfig()->get("cost.discount_rate", 0.0));

        if (discount_rate != 0.0) {
            respond::data_ops::CostList disc_costs =
                respond::model::CalculateCosts(
                    history, *cost_loader,
                    data_loader.GetConfig()->getStringVector(
                        "cost.cost_perspectives"),
                    true, discount_rate);

            total_disc_costs = respond::model::CalculateTotalCosts(disc_costs);

            respond::data_ops::TimedMatrix3d disc_utilities =
                respond::model::CalculateUtilities(
                    history, *utility_loader,
                    respond::data_ops::UtilityType::kMin, true, discount_rate);
            total_disc_utility =
                respond::data_ops::TimedMatrix3dSummedOverDimensions(
                    disc_utilities);

            disc_life_years = respond::model::CalculateLifeYears(history, true,
                                                                 discount_rate);
        }

        totals.base_costs = respond::model::CalculateTotalCosts(base_costs);
        totals.base_life_years = respond::model::CalculateLifeYears(history);
        totals.base_utility =
            respond::data_ops::TimedMatrix3dSummedOverDimensions(
                base_utilities);
        totals.disc_costs = total_disc_costs;
        totals.disc_life_years = disc_life_years;
        totals.disc_utility = total_disc_utility;
    }

    auto formatter = respond::data_ops::DataFormatter::Create();

    std::vector<int> timesteps = data_loader.GetConfig()->getIntVector(
        "output.general_stats_output_timesteps");
    formatter->ExtractTimesteps(timesteps, history, base_costs, base_utilities,
                                std::get<bool>(data_loader.GetConfig()->get(
                                    "cost.cost_analysis", false)));

    auto writer =
        respond::data_ops::Writer::Create(data_loader.GetConfig(), logger_name);

    if (std::get<bool>(data_loader.GetConfig()->get(
            "output.write_calibrated_inputs", false))) {
        writer->WriteInputData(data_loader, output_directory,
                               respond::data_ops::OutputType::kFile);
    } else {
        writer->WriteHistoryData(history, output_directory,
                                 respond::data_ops::OutputType::kFile);
        writer->WriteCostData(base_costs, output_directory,
                              respond::data_ops::OutputType::kFile);
        writer->WriteUtilityData(base_utilities, output_directory,
                                 respond::data_ops::OutputType::kFile);
        writer->WriteTotalsData(totals, output_directory,
                                respond::data_ops::OutputType::kFile);
    }
}

void execute(int argc, char **argv) {
    int start;
    int end;
    std::string root;
    if (!ArgChecks(argc, argv, root, start, end)) {
        return;
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
            std::string logger_name = "logger" + std::to_string(i);
            respond::utils::CreateFileLogger(logger_name, log_path);

            respond::utils::LogInfo(logger_name, "Logger Created");

            auto data_loader = respond::data_ops::DataLoader::Create(
                input_set.string(), logger_name);
            respond::utils::LogInfo(logger_name, "DataLoader Created");

            LoadDataLoader(*data_loader);

            auto respond = respond::model::Respond::Create(logger_name);
            respond->Run(*data_loader);
            respond::data_ops::History history = respond->GetHistory();

            DoPostSimulationCalculations(
                *data_loader, history, input_set.string(),
                output_directory.string(), logger_name);

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "Simulation Complete! :)" << std::endl;
    return;
}

int main(int argc, char **argv) { execute(argc, argv); }