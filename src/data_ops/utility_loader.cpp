////////////////////////////////////////////////////////////////////////////////
// File: utility_loader.cpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/utility_loader_internals.hpp"

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond::data_ops {
    std::unordered_map<std::string, Matrix3d>
    UtilityLoaderImpl::LoadBackgroundUtility(const std::string &file) {
        this->background_utility = this->LoadUtility(file);
        return this->background_utility;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoaderImpl::LoadOUDUtility(const std::string &file) {
        this->behavior_utility = this->LoadUtility(file);
        return this->behavior_utility;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoaderImpl::LoadSettingUtility(const std::string &file) {
        this->setting_utility = this->LoadUtility(file);
        return this->setting_utility;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoaderImpl::LoadUtility(const std::string &file) {
        Data::IDataTablePtr table = LoadDataTable(file);
        std::unordered_map<std::string, Matrix3d> result;

        size_t number_behavior_states =
            GetConfig()->getStringVector("state.ouds").size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        size_t number_intervention_states =
            GetConfig()->getStringVector("state.interventions").size();

        Matrix3d utilMatrix =
            CreateMatrix3d(number_behavior_states, number_intervention_states,
                           number_demographic_combos);

        std::vector<std::string> utilCol = table->getColumn("utility");

        for (int i = 0; i < number_intervention_states; ++i) {
            Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extent = utilMatrix.dimensions();
            offset[Dimension::kIntervention] = i;
            extent[Dimension::kIntervention] = 1;
            Matrix3d temp = utilMatrix.slice(offset, extent);
            if (utilCol.size() > i) {
                temp.setConstant(std::stod(utilCol[i]));
            } else {
                temp.setConstant(0.0);
            }

            utilMatrix.slice(offset, extent) = temp;
        }
        result["utility"] = utilMatrix;

        return result;
    }
} // namespace respond::data_ops