////////////////////////////////////////////////////////////////////////////////
// File: data_formatter.cpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/data_formatter_internals.hpp"

#include <algorithm>
#include <vector>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond {
namespace data_ops {
void DataFormatterImpl::ExtractTimesteps(std::vector<int> &timesteps,
                                         History &history, CostList &costs,
                                         TimedMatrix3d &utilities,
                                         bool costSwitch) {
    if (timesteps.size() == 0) {
        return;
    }

    std::sort(timesteps.begin(), timesteps.end());

    history.state_history = TrimTimedMatrix3d(timesteps, history.state_history);

    history.fatal_overdose_history =
        TrimAndAddTimedMatrix3d(timesteps, history.fatal_overdose_history);

    history.intervention_admission_history = TrimAndAddTimedMatrix3d(
        timesteps, history.intervention_admission_history);

    history.mortality_history =
        TrimAndAddTimedMatrix3d(timesteps, history.mortality_history);

    history.overdose_history =
        TrimAndAddTimedMatrix3d(timesteps, history.overdose_history);

    if (costSwitch) {
        for (Cost &cost : costs) {
            cost.fatal_overdose_cost =
                TrimAndAddTimedMatrix3d(timesteps, cost.fatal_overdose_cost);
            cost.healthcare_cost =
                TrimAndAddTimedMatrix3d(timesteps, cost.healthcare_cost);
            cost.non_fatal_overdose_cost = TrimAndAddTimedMatrix3d(
                timesteps, cost.non_fatal_overdose_cost);
            cost.pharma_cost =
                TrimAndAddTimedMatrix3d(timesteps, cost.pharma_cost);
            cost.treatment_cost =
                TrimAndAddTimedMatrix3d(timesteps, cost.treatment_cost);
        }

        utilities = TrimAndAddTimedMatrix3d(timesteps, utilities);
    }
}

TimedMatrix3d
DataFormatterImpl::TrimTimedMatrix3d(const std::vector<int> &timesteps,
                                     const TimedMatrix3d &matrix) {
    TimedMatrix3d trimmed;
    for (int timestep : timesteps) {
        trimmed[timestep] = matrix.at(timestep);
    }
    return trimmed;
}

TimedMatrix3d
DataFormatterImpl::TrimAndAddTimedMatrix3d(const std::vector<int> &timesteps,
                                           const TimedMatrix3d &matrices) {
    auto itr = matrices.begin();
    if (itr == matrices.end() || timesteps.empty()) {
        // empty matrix or timesteps
        return {};
    }
    Matrix3d temp = itr->second;
    int number_of_behaviors = temp.dimension(static_cast<int>(Dimension::kOud));
    int number_of_interventions =
        temp.dimension(static_cast<int>(Dimension::kIntervention));
    int number_of_demographic_combinations =
        temp.dimension(static_cast<int>(Dimension::kDemographicCombo));

    TimedMatrix3d trimmed;
    Matrix3d running =
        CreateMatrix3d(number_of_interventions, number_of_behaviors,
                       number_of_demographic_combinations);

    int timestep_index = 0;
    for (auto kv : matrices) {
        running += kv.second;
        if (kv.first >= timesteps[timestep_index]) {
            trimmed[kv.first] = running;
            running =
                CreateMatrix3d(number_of_interventions, number_of_behaviors,
                               number_of_demographic_combinations);
            timestep_index++;
        }
    }
    return trimmed;
}

std::unique_ptr<DataFormatter> DataFormatter::Create() {
    return std::make_unique<DataFormatterImpl>();
}
} // namespace data_ops
} // namespace respond