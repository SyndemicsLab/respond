////////////////////////////////////////////////////////////////////////////////
// File: cost_loader.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-11                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/cost_loader_internals.hpp"

#include <respond/data_ops/matrix_3d_factory.hpp>

namespace respond::data_ops {

    std::unordered_map<std::string, Matrix3d>
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

            std::vector<std::string> healthColumn =
                table->getColumn(perspective);

            if (healthColumn.empty()) {
                respond::utils::LogError(logger_name,
                                         "No data found for perspective: " +
                                             perspective);
                return {};
            }

            healthcare_utilization_cost[perspective] = Matrix3dFactory::Create(
                number_behavior_states, number_intervention_states,
                number_demographic_combos);

            int rowIdx = 0;
            for (int intervention = 0;
                 intervention < number_intervention_states; ++intervention) {
                for (int dem = 0; dem < number_demographic_combos; ++dem) {
                    for (int oud_state = 0; oud_state < number_behavior_states;
                         ++oud_state) {
                        double cost = (healthColumn.size() > rowIdx)
                                          ? std::stod(healthColumn[rowIdx])
                                          : 0.0;
                        healthcare_utilization_cost[perspective](
                            intervention, oud_state, dem) = cost;
                        rowIdx++;
                    }
                }
            }
        }
        return healthcare_utilization_cost;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
    CostLoaderImpl::LoadOverdoseCost(const std::string &file) {
        Data::IDataTablePtr table = LoadDataTable(file);

        std::vector<std::string> xCol = table->getColumn("X");

        std::vector<std::string> cost_perspectives =
            GetConfig()->getStringVector("cost.cost_perspectives");
        for (std::string perspective : cost_perspectives) {
            for (size_t i = 0; i < xCol.size(); i++) {
                std::vector<std::string> persCol =
                    table->getColumn(perspective);
                if (persCol.empty()) {
                    respond::utils::LogError(logger_name,
                                             "Cost perspective " + perspective +
                                                 " not found in table");
                }
                overdose_costs_map[perspective][xCol[i]] =
                    std::stod(persCol[i]);
            }
        }
        return overdose_costs_map;
    }

    std::unordered_map<std::string, Matrix3d>
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

    std::unordered_map<std::string, Matrix3d>
    CostLoaderImpl::LoadTreatmentUtilizationCost(const std::string &file) {
        Data::IDataTablePtr table = LoadDataTable(file);
        LoadTreatmentUtilizationCostMap(table);

        LoadCostViaPerspective(treatment_utilization_cost,
                               treatment_utilization_cost_map);
        return treatment_utilization_cost;
    }

    double CostLoaderImpl::GetNonFatalOverdoseCost(
        const std::string &perspective) const {
        if (overdose_costs_map.at(perspective).find("non_fatal_overdose") ==
            overdose_costs_map.at(perspective).end()) {
            return 0.0;
        }
        return overdose_costs_map.at(perspective).at("non_fatal_overdose");
    }

    double
    CostLoaderImpl::GetFatalOverdoseCost(const std::string &perspective) const {
        if (overdose_costs_map.at(perspective).find("fatal_overdose") ==
            overdose_costs_map.at(perspective).end()) {
            return 0.0;
        }
        return overdose_costs_map.at(perspective).at("fatal_overdose");
    }

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
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
        std::unordered_map<std::string, Matrix3d> &cost,
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            &cost_map) {

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
                Matrix3dFactory::Create(number_behavior_states,
                                        number_intervention_states,
                                        number_demographic_combos)
                    .constant(0);

            for (int i = 0; i < number_intervention_states; ++i) {
                Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> extent =
                    cost[perspective].dimensions();
                offset[Dimension::kIntervention] = i;
                extent[Dimension::kIntervention] = 1;
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

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
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

} // namespace respond::data_ops
