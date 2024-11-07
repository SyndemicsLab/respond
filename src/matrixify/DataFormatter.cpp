#include "DataFormatter.hpp"

namespace matrixify {
    void DataFormatter::extractTimesteps(std::vector<int> timesteps,
                                         matrixify::History &history,
                                         matrixify::CostList &costs,
                                         matrixify::Matrix4d &utilities,
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
            for (matrixify::Cost &cost : costs) {
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

    matrixify::Matrix4d
    DataFormatter::trimMatrix4d(std::vector<int> timesteps,
                                matrixify::Matrix4d matrix) {
        matrixify::Matrix4d trimmed;
        for (int timestep : timesteps) {
            trimmed.insert(matrix.getMappedData()[timestep], timestep);
        }
        return trimmed;
    }

    matrixify::Matrix4d
    DataFormatter::trimAndAddMatrix4d(std::vector<int> timesteps,
                                      matrixify::Matrix4d matrix) {
        matrixify::Matrix3d temp = matrix.getMatrix3dAtTimestep(0);
        int oudSize = temp.dimension(matrixify::OUD);
        int interSize = temp.dimension(matrixify::INTERVENTION);
        int demSize = temp.dimension(matrixify::DEMOGRAPHIC_COMBO);

        matrixify::Matrix4d trimmed;
        matrixify::Matrix3d running =
            matrixify::Matrix3dFactory::Create(oudSize, interSize, demSize);

        std::vector<matrixify::Matrix3d> matrices = matrix.getMatrices();
        int timestepIdx = 0;
        for (int i = 0; i < matrices.size(); ++i) {
            if (timesteps.size() <= timestepIdx) {
                break;
            }
            running += matrices[i];
            if (i >= timesteps[timestepIdx]) {
                trimmed.insert(running, timesteps[timestepIdx]);
                running = matrixify::Matrix3dFactory::Create(oudSize, interSize,
                                                             demSize);
                timestepIdx++;
            }
        }

        return trimmed;
    }
} // namespace matrixify