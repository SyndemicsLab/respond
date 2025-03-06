////////////////////////////////////////////////////////////////////////////////
// File: DataFormatter.cpp                                                    //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respondsimulation/data_ops/DataFormatter.hpp>

namespace Matrixify {
    void DataFormatter::extractTimesteps(std::vector<int> timesteps,
                                         Matrixify::History &history,
                                         Matrixify::CostList &costs,
                                         Matrixify::Matrix4d &utilities,
                                         bool costSwitch) {
        if (timesteps.size() == 0) {
            return;
        }

        std::sort(timesteps.begin(), timesteps.end());

        history.stateHistory = trimMatrix4d(timesteps, history.stateHistory);

        history.fatalOverdoseHistory =
            trimAndAddMatrix4d(timesteps, history.fatalOverdoseHistory);

        history.interventionAdmissionHistory =
            trimAndAddMatrix4d(timesteps, history.interventionAdmissionHistory);

        history.mortalityHistory =
            trimAndAddMatrix4d(timesteps, history.mortalityHistory);

        history.overdoseHistory =
            trimAndAddMatrix4d(timesteps, history.overdoseHistory);

        if (costSwitch) {
            for (Matrixify::Cost &cost : costs) {
                cost.fatalOverdoseCost =
                    trimAndAddMatrix4d(timesteps, cost.fatalOverdoseCost);
                cost.healthcareCost =
                    trimAndAddMatrix4d(timesteps, cost.healthcareCost);
                cost.nonFatalOverdoseCost =
                    trimAndAddMatrix4d(timesteps, cost.nonFatalOverdoseCost);
                cost.pharmaCost =
                    trimAndAddMatrix4d(timesteps, cost.pharmaCost);
                cost.treatmentCost =
                    trimAndAddMatrix4d(timesteps, cost.treatmentCost);
            }

            utilities = trimAndAddMatrix4d(timesteps, utilities);
        }
    }

    Matrixify::Matrix4d
    DataFormatter::trimMatrix4d(std::vector<int> timesteps,
                                Matrixify::Matrix4d matrix) {
        Matrixify::Matrix4d trimmed;
        for (int timestep : timesteps) {
            trimmed.insert(matrix.getMappedData()[timestep], timestep);
        }
        return trimmed;
    }

    Matrixify::Matrix4d
    DataFormatter::trimAndAddMatrix4d(std::vector<int> timesteps,
                                      Matrixify::Matrix4d matrix) {
        Matrixify::Matrix3d temp = matrix.getMatrix3dAtTimestep(0);
        int oudSize = temp.dimension(Matrixify::OUD);
        int interSize = temp.dimension(Matrixify::INTERVENTION);
        int demSize = temp.dimension(Matrixify::DEMOGRAPHIC_COMBO);

        Matrixify::Matrix4d trimmed;
        Matrixify::Matrix3d running =
            Matrixify::Matrix3dFactory::Create(oudSize, interSize, demSize);

        std::vector<Matrixify::Matrix3d> matrices = matrix.getMatrices();
        int timestepIdx = 0;
        for (int i = 0; i < matrices.size(); ++i) {
            if (timesteps.size() <= timestepIdx) {
                break;
            }
            running += matrices[i];
            if (i >= timesteps[timestepIdx]) {
                trimmed.insert(running, timesteps[timestepIdx]);
                running = Matrixify::Matrix3dFactory::Create(oudSize, interSize,
                                                             demSize);
                timestepIdx++;
            }
        }

        return trimmed;
    }
} // namespace Matrixify