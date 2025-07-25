////////////////////////////////////////////////////////////////////////////////
// File: exec.cpp                                                             //
// Project: executable                                                        //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-24                                                  //
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

void LoadDataLoader(respond::preprocess::DataLoader &data_loader,
                    const std::filesystem::path &directory) {
    data_loader.SetConfig((directory / "sim.conf").string());
    data_loader.LoadInitialSample((directory / "init_cohort.csv").string());
    if (std::get<bool>(data_loader.GetConfig()->get(
            "simulation.stratified_entering_cohort", false))) {
        data_loader.LoadEnteringSamples(
            (directory / "entering_cohort.csv").string());
    } else {
        data_loader.LoadEnteringSamples(
            (directory / "entering_cohort.csv").string(), "No_Treatment",
            "Active_Noninjection");
    }
    data_loader.LoadOUDTransitionRates((directory / "oud_trans.csv").string());
    data_loader.LoadInterventionInitRates(
        (directory / "block_init_effect.csv").string());
    data_loader.LoadInterventionTransitionRates(
        (directory / "block_trans.csv").string());
    data_loader.LoadOverdoseRates(
        (directory / "all_types_overdose.csv").string());
    data_loader.LoadFatalOverdoseRates(
        (directory / "fatal_overdose.csv").string());
    data_loader.LoadMortalityRates(
        (directory / "SMR.csv").string(),
        (directory / "background_mortality.csv").string());
}

void LoadCostLoader(respond::preprocess::CostLoader &cost_loader,
                    const std::filesystem::path &directory) {
    cost_loader.SetConfig((directory / "sim.conf").string());
    cost_loader.LoadHealthcareUtilizationCost(
        (directory / "healthcare_utilization_cost.csv").string());
    cost_loader.LoadOverdoseCost((directory / "overdose_cost.csv").string());
    cost_loader.LoadPharmaceuticalCost(
        (directory / "pharmaceutical_cost.csv").string());
    cost_loader.LoadTreatmentUtilizationCost(
        (directory / "treatment_utilization_cost.csv").string());
}

void LoadUtilityLoader(respond::preprocess::UtilityLoader &utility_loader,
                       const std::filesystem::path &directory) {
    utility_loader.SetConfig((directory / "sim.conf").string());
    utility_loader.LoadBackgroundUtility(
        (directory / "bg_utility.csv").string());
    utility_loader.LoadOUDUtility((directory / "oud_utility.csv").string());
    utility_loader.LoadSettingUtility(
        (directory / "setting_utility.csv").string());
}

void DoPostSimulationCalculations(
    const respond::preprocess::DataLoader &data_loader,
    respond::preprocess::History history, const std::string &input_set,
    const std::string &output_directory, const std::string &logger_name) {
    if (!std::get<bool>(
            data_loader.GetConfig()->get("cost.cost_analysis", false))) {
        return;
    }
    auto cost_loader = respond::preprocess::CostLoader::Create(logger_name);
    respond::utils::LogInfo(logger_name, "CostLoader Created");

    LoadCostLoader(*cost_loader, input_set);

    auto utility_loader =
        respond::preprocess::UtilityLoader::Create(logger_name);
    respond::utils::LogInfo(logger_name, "UtilityLoader Created");

    LoadUtilityLoader(*utility_loader, input_set);

    respond::preprocess::CostList base_costs;
    respond::preprocess::TimedMatrix3d base_utilities;
    respond::preprocess::Totals totals;

    if (std::get<bool>(
            data_loader.GetConfig()->get("cost.cost_analysis", false))) {

        double disc_life_years = 0.0;
        std::vector<double> total_disc_costs = {};
        double total_disc_utility = 0.0;

        base_costs = respond::model::CalculateCosts(
            history, *cost_loader,
            data_loader.GetConfig()->getStringVector("cost.cost_perspectives"));

        base_utilities = respond::model::CalculateUtilities(
            history, *utility_loader, respond::preprocess::UtilityType::kMin);

        double discount_rate = std::get<double>(
            data_loader.GetConfig()->get("cost.discount_rate", 0.0));

        if (discount_rate != 0.0) {
            respond::preprocess::CostList disc_costs =
                respond::model::CalculateCosts(
                    history, *cost_loader,
                    data_loader.GetConfig()->getStringVector(
                        "cost.cost_perspectives"),
                    true, discount_rate);

            total_disc_costs = respond::model::CalculateTotalCosts(disc_costs);

            respond::preprocess::TimedMatrix3d disc_utilities =
                respond::model::CalculateUtilities(
                    history, *utility_loader,
                    respond::preprocess::UtilityType::kMin, true,
                    discount_rate);
            total_disc_utility =
                respond::preprocess::TimedMatrix3dSummedOverDimensions(
                    disc_utilities);

            disc_life_years = respond::model::CalculateLifeYears(history, true,
                                                                 discount_rate);
        }

        totals.base_costs = respond::model::CalculateTotalCosts(base_costs);
        totals.base_life_years = respond::model::CalculateLifeYears(history);
        totals.base_utility =
            respond::preprocess::TimedMatrix3dSummedOverDimensions(
                base_utilities);
        totals.disc_costs = total_disc_costs;
        totals.disc_life_years = disc_life_years;
        totals.disc_utility = total_disc_utility;
    }

    auto formatter = respond::preprocess::DataFormatter::Create();

    std::vector<int> timesteps = data_loader.GetConfig()->getIntVector(
        "output.general_stats_output_timesteps");
    formatter->ExtractTimesteps(timesteps, history, base_costs, base_utilities,
                                std::get<bool>(data_loader.GetConfig()->get(
                                    "cost.cost_analysis", false)));

    auto writer = respond::preprocess::Writer::Create(input_set, logger_name);

    writer->WriteCostData(base_costs, output_directory,
                          respond::preprocess::OutputType::kFile);
    writer->WriteUtilityData(base_utilities, output_directory,
                             respond::preprocess::OutputType::kFile);
    writer->WriteTotalsData(totals, output_directory,
                            respond::preprocess::OutputType::kFile);
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

            auto data_loader =
                respond::preprocess::DataLoader::Create(logger_name);
            respond::utils::LogInfo(logger_name, "DataLoader Created");

            LoadDataLoader(*data_loader, input_set);

            auto respond = respond::model::Respond::Create(logger_name);
            respond->Run(*data_loader);
            respond::preprocess::History history = respond->GetHistory();

            DoPostSimulationCalculations(
                *data_loader, history, input_set.string(),
                output_directory.string(), logger_name);

            auto writer = respond::preprocess::Writer::Create(
                input_set.string(), logger_name);

            if (std::get<bool>(data_loader->GetConfig()->get(
                    "output.write_calibrated_inputs", false))) {
                writer->WriteInputData(*data_loader, output_directory,
                                       respond::preprocess::OutputType::kFile);
            } else {

                writer->WriteHistoryData(
                    history, output_directory,
                    respond::preprocess::OutputType::kFile);
            }

            std::cout << "Output " << std::to_string(i) << " Complete"
                      << std::endl;
        });
    std::cout << "Simulation Complete!" << std::endl;
    return;
}

int main(int argc, char **argv) { execute(argc, argv); }