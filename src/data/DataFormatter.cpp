#include "DataFormatter.hpp"

namespace Data {
    void DataFormatter::extractTimesteps(std::vector<int> timesteps,
                                         Data::History &history,
                                         Data::Costs &costs,
                                         Data::Utility &utility,
                                         bool costSwitch) {
        if (timesteps.size() == 0) {
            return;
        }

        std::sort(timesteps.begin(), timesteps.end());

        history.stateHistory =
            trimMatrix3dOverTime(timesteps, history.stateHistory);

        history.fatalOverdoseHistory =
            trimAndAddMatrix3dOverTime(timesteps, history.fatalOverdoseHistory);

        history.interventionAdmissionHistory = trimAndAddMatrix3dOverTime(
            timesteps, history.interventionAdmissionHistory);

        history.mortalityHistory =
            trimAndAddMatrix3dOverTime(timesteps, history.mortalityHistory);

        history.overdoseHistory =
            trimAndAddMatrix3dOverTime(timesteps, history.overdoseHistory);

        if (costSwitch) {
            for (Data::Cost &cost : costs) {
                cost.fatalOverdoseCost = trimAndAddMatrix3dOverTime(
                    timesteps, cost.fatalOverdoseCost);
                cost.healthcareCost =
                    trimAndAddMatrix3dOverTime(timesteps, cost.healthcareCost);
                cost.nonFatalOverdoseCost = trimAndAddMatrix3dOverTime(
                    timesteps, cost.nonFatalOverdoseCost);
                cost.pharmaCost =
                    trimAndAddMatrix3dOverTime(timesteps, cost.pharmaCost);
                cost.treatmentCost =
                    trimAndAddMatrix3dOverTime(timesteps, cost.treatmentCost);
            }

            utility.backgroundUtility = trimAndAddMatrix3dOverTime(
                timesteps, utility.backgroundUtility);
            utility.oudUtility =
                trimAndAddMatrix3dOverTime(timesteps, utility.oudUtility);
            utility.settingUtility =
                trimAndAddMatrix3dOverTime(timesteps, utility.settingUtility);
        }
    }

    Data::Matrix3dOverTime
    DataFormatter::trimMatrix3dOverTime(std::vector<int> timesteps,
                                        Data::Matrix3dOverTime matrix) {
        Data::Matrix3dOverTime trimmed;
        for (int timestep : timesteps) {
            trimmed.insert(matrix.getMappedData()[timestep], timestep);
        }
        return trimmed;
    }

    Data::Matrix3dOverTime
    DataFormatter::trimAndAddMatrix3dOverTime(std::vector<int> timesteps,
                                              Data::Matrix3dOverTime matrix) {
        Data::Matrix3d temp = matrix.getMatrix3dAtTimestep(0);
        int oudSize = temp.dimension(Data::OUD);
        int interSize = temp.dimension(Data::INTERVENTION);
        int demSize = temp.dimension(Data::DEMOGRAPHIC_COMBO);

        Data::Matrix3dOverTime trimmed;
        Data::Matrix3d running =
            Utilities::Matrix3dFactory::Create(oudSize, interSize, demSize)
                .setZero();

        std::vector<Data::Matrix3d> matrices = matrix.getMatrices();
        int timestepIdx = 0;
        for (int i = 0; i < matrices.size(); ++i) {
            if (timesteps.size() <= timestepIdx) {
                break;
            }
            running += matrices[i];
            if (i >= timesteps[timestepIdx]) {
                trimmed.insert(running, timesteps[timestepIdx]);
                running = Utilities::Matrix3dFactory::Create(oudSize, interSize,
                                                             demSize)
                              .setZero();
                timestepIdx++;
            }
        }

        return trimmed;
    }
} // namespace Data