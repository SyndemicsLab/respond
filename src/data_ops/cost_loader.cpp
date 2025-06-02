////////////////////////////////////////////////////////////////////////////////
// File: cost_loader.cpp                                                      //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/cost_loader_internals.hpp"

#include <string>
#include <unordered_map>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>
#include <respond/utils/logging.hpp>

namespace respond {
namespace data_ops {

StringUOMap<Matrix3d>
CostLoaderImpl::LoadHealthcareUtilizationCost(const std::string &file) {
    Data::IDataTablePtr table = LoadDataTable(file);

    size_t number_behavior_states =
        GetConfig()->getStringVector("state.ouds").size();
    size_t number_demographic_combos = GetDemographicCombos().size();
    size_t number_intervention_states =
        GetConfig()->getStringVector("state.interventions").size();

    std::vector<std::string> cost_perspectives =
        GetConfig()->getStringVector("cost.cost_perspectives");
    for (std::string perspective : cost_perspectives) {

        std::vector<std::string> column = table->getColumn(perspective);

        if (column.empty()) {
            respond::utils::LogError(
                logger_name, "No data found for perspective: " + perspective);
            return {};
        }

        healthcare_utilization_cost[perspective] =
            CreateMatrix3d(number_intervention_states, number_behavior_states,
                           number_demographic_combos);

        int idx = 0;
        for (int i = 0; i < number_intervention_states; ++i) {
            for (int d = 0; d < number_demographic_combos; ++d) {
                for (int b = 0; b < number_behavior_states; ++b) {
                    double cost =
                        (column.size() > idx) ? std::stod(column[idx]) : 0.0;
                    healthcare_utilization_cost[perspective](i, b, d) = cost;
                    idx++;
                }
            }
        }
    }
    return healthcare_utilization_cost;
}

StringUOMap<StringUOMap<double>>
CostLoaderImpl::LoadOverdoseCost(const std::string &file) {
    Data::IDataTablePtr table = LoadDataTable(file);

    std::vector<std::string> x_column = table->getColumn("X");

    std::vector<std::string> cost_perspectives =
        GetConfig()->getStringVector("cost.cost_perspectives");
    for (std::string perspective : cost_perspectives) {
        for (size_t i = 0; i < x_column.size(); i++) {
            std::vector<std::string> perspective_column =
                table->getColumn(perspective);
            if (perspective_column.empty()) {
                respond::utils::LogError(
                    logger_name, "Cost perspective " + perspective +
                                     " not found in table! Returning...");
                return {};
            }
            overdose_costs_map[perspective][x_column[i]] =
                std::stod(perspective_column[i]);
        }
    }
    return overdose_costs_map;
}

StringUOMap<Matrix3d>
CostLoaderImpl::LoadPharmaceuticalCost(const std::string &file) {
    Data::IDataTablePtr table = LoadDataTable(file);

    size_t number_behavior_states =
        GetConfig()->getStringVector("state.ouds").size();
    size_t number_demographic_combos = GetDemographicCombos().size();
    size_t number_intervention_states =
        GetConfig()->getStringVector("state.interventions").size();

    LoadPharmaceuticalCostMap(table);

    LoadCostViaPerspective(pharmaceutical_cost, pharmaceutical_costs_map);

    return pharmaceutical_cost;
}

StringUOMap<Matrix3d>
CostLoaderImpl::LoadTreatmentUtilizationCost(const std::string &file) {
    Data::IDataTablePtr table = LoadDataTable(file);
    LoadTreatmentUtilizationCostMap(table);

    LoadCostViaPerspective(treatment_utilization_cost,
                           treatment_utilization_cost_map);
    return treatment_utilization_cost;
}

double
CostLoaderImpl::GetNonFatalOverdoseCost(const std::string &perspective) const {
    if (overdose_costs_map.at(perspective).find("non_fatal_overdose") ==
        overdose_costs_map.at(perspective).end()) {
        respond::utils::LogWarning(
            logger_name, "No Non Fatal Overdose Costs Found. Returning 0...");
        return 0.0;
    }
    return overdose_costs_map.at(perspective).at("non_fatal_overdose");
}

double
CostLoaderImpl::GetFatalOverdoseCost(const std::string &perspective) const {
    if (overdose_costs_map.at(perspective).find("fatal_overdose") ==
        overdose_costs_map.at(perspective).end()) {
        respond::utils::LogWarning(
            logger_name, "No Fatal Overdose Costs Found. Return 0...");
        return 0.0;
    }
    return overdose_costs_map.at(perspective).at("fatal_overdose");
}

StringUOMap<StringUOMap<double>>
CostLoaderImpl::LoadPharmaceuticalCostMap(Data::IDataTablePtr table) {
    std::vector<std::string> block_column = table->getColumn("block");

    std::vector<std::string> cost_perspectives =
        GetConfig()->getStringVector("cost.cost_perspectives");
    for (std::string perspective : cost_perspectives) {
        std::vector<std::string> perspective_column =
            table->getColumn(perspective);
        for (size_t i = 0; i < block_column.size(); i++) {
            pharmaceutical_costs_map[perspective][block_column[i]] =
                std::stod(perspective_column[i]);
        }
    }
    return pharmaceutical_costs_map;
}

void CostLoaderImpl::LoadCostViaPerspective(
    StringUOMap<Matrix3d> &cost, StringUOMap<StringUOMap<double>> &cost_map) {

    size_t number_behavior_states =
        GetConfig()->getStringVector("state.ouds").size();
    size_t number_demographic_combos = GetDemographicCombos().size();

    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    size_t number_intervention_states = interventions.size();

    std::vector<std::string> cost_perspectives =
        GetConfig()->getStringVector("cost.cost_perspectives");
    for (std::string perspective : cost_perspectives) {
        cost[perspective] =
            CreateMatrix3d(number_intervention_states, number_behavior_states,
                           number_demographic_combos);

        for (int i = 0; i < number_intervention_states; ++i) {
            Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extent =
                cost[perspective].dimensions();
            offset[static_cast<int>(Dimension::kIntervention)] = i;
            extent[static_cast<int>(Dimension::kIntervention)] = 1;
            Matrix3d slice = cost[perspective].slice(offset, extent);
            if (cost_map[perspective].find(interventions[i]) !=
                cost_map[perspective].end()) {
                slice.setConstant(cost_map[perspective][interventions[i]]);
            } else {
                slice.setConstant(0.0);
            }
            cost[perspective].slice(offset, extent) = slice;
        }
    }
}

StringUOMap<StringUOMap<double>>
CostLoaderImpl::LoadTreatmentUtilizationCostMap(Data::IDataTablePtr table) {
    std::vector<std::string> block_column = table->getColumn("block");

    std::vector<std::string> cost_perspectives =
        GetConfig()->getStringVector("cost.cost_perspectives");
    for (std::string perspective : cost_perspectives) {
        std::vector<std::string> perspective_column =
            table->getColumn(perspective);
        for (size_t i = 0; i < block_column.size(); i++) {
            treatment_utilization_cost_map[perspective][block_column[i]] =
                std::stod(perspective_column[i]);
        }
    }
    return treatment_utilization_cost_map;
}

std::unique_ptr<CostLoader> CostLoader::Create(const std::string &directory,
                                               const std::string &log_name) {
    return std::make_unique<CostLoaderImpl>(directory, log_name);
}

} // namespace data_ops
} // namespace respond
