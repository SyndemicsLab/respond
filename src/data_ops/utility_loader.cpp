////////////////////////////////////////////////////////////////////////////////
// File: utility_loader.cpp                                                   //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/utility_loader_internals.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include <Eigen/Eigen>
#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond {
namespace data_ops {
StringUOMap<Matrix3d>
UtilityLoaderImpl::LoadBackgroundUtility(const std::string &file) {
    this->background_utility = this->LoadUtility(file);
    return this->background_utility;
}

StringUOMap<Matrix3d>
UtilityLoaderImpl::LoadOUDUtility(const std::string &file) {
    this->behavior_utility = this->LoadUtility(file);
    return this->behavior_utility;
}

StringUOMap<Matrix3d>
UtilityLoaderImpl::LoadSettingUtility(const std::string &file) {
    this->setting_utility = this->LoadUtility(file);
    return this->setting_utility;
}

StringUOMap<Matrix3d> UtilityLoaderImpl::LoadUtility(const std::string &file) {
    Data::IDataTablePtr table = LoadDataTable(file);
    StringUOMap<Matrix3d> result;

    size_t number_behavior_states =
        GetConfig()->getStringVector("state.ouds").size();
    size_t number_demographic_combos = GetDemographicCombos().size();
    size_t number_intervention_states =
        GetConfig()->getStringVector("state.interventions").size();

    Matrix3d utilMatrix =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);

    std::vector<std::string> utilCol = table->getColumn("utility");

    for (int i = 0; i < number_intervention_states; ++i) {
        Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
        Eigen::array<Eigen::Index, 3> extent = utilMatrix.dimensions();
        offset[static_cast<int>(Dimension::kIntervention)] = i;
        extent[static_cast<int>(Dimension::kIntervention)] = 1;
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

std::unique_ptr<UtilityLoader>
UtilityLoader::Create(const std::string &directory,
                      const std::string &log_name) {
    return std::make_unique<UtilityLoaderImpl>(directory, log_name);
}
} // namespace data_ops
} // namespace respond