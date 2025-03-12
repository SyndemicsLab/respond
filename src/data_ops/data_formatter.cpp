////////////////////////////////////////////////////////////////////////////////
// File: data_formatter.cpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/data_formatter_internals.hpp"

namespace respond::data_ops {
    void DataFormatterImpl::ExtractTimesteps(std::vector<int> timesteps,
                                             History &history, CostList &costs,
                                             TimedMatrix3d &utilities,
                                             bool costSwitch) {
        if (timesteps.size() == 0) {
            return;
        }

        std::sort(timesteps.begin(), timesteps.end());

        history.state_history =
            TrimTimedMatrix3d(timesteps, history.state_history);

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
                cost.fatal_overdose_cost = TrimAndAddTimedMatrix3d(
                    timesteps, cost.fatal_overdose_cost);
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
    DataFormatterImpl::TrimTimedMatrix3d(std::vector<int> timesteps,
                                         TimedMatrix3d matrix) {
        TimedMatrix3d trimmed;
        for (int timestep : timesteps) {
            trimmed.Insert(matrix.GetMappedData()[timestep], timestep);
        }
        return trimmed;
    }

    TimedMatrix3d
    DataFormatterImpl::TrimAndAddTimedMatrix3d(std::vector<int> timesteps,
                                               TimedMatrix3d matrix) {
        Matrix3d temp = matrix.GetMatrix3dAtTimestep(0);
        int oudSize = temp.dimension(Dimension::kOud);
        int interSize = temp.dimension(Dimension::kIntervention);
        int demSize = temp.dimension(Dimension::kDemographicCombo);

        TimedMatrix3d trimmed;
        Matrix3d running = Matrix3dFactory::Create(oudSize, interSize, demSize);

        std::vector<Matrix3d> matrices = matrix.GetMatrices();
        int timestepIdx = 0;
        for (int i = 0; i < matrices.size(); ++i) {
            if (timesteps.size() <= timestepIdx) {
                break;
            }
            running += matrices[i];
            if (i >= timesteps[timestepIdx]) {
                trimmed.Insert(running, timesteps[timestepIdx]);
                running = Matrix3dFactory::Create(oudSize, interSize, demSize);
                timestepIdx++;
            }
        }

        return trimmed;
    }
} // namespace respond::data_ops