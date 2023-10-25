//===-- CostCalculator.cpp - CostCalculator class definition ----*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the CostCalculator class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:33 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "PostSimulationCalculator.hpp"

namespace Calculator {

    PostSimulationCalculator::PostSimulationCalculator(
        Data::CostLoader const &costLoader,
        Data::UtilityLoader const &utilityLoader,
        Data::History const &history) {
        this->history = history;
        this->costLoader = costLoader;
        this->utilityLoader = utilityLoader;
    }

    Data::Costs PostSimulationCalculator::calculateCosts() const {
        Data::Costs costs;
        std::vector<std::string> perspectives =
            this->costLoader.getCostPerspectives();

        for (std::string perspective : perspectives) {
            Data::Cost cost;
            cost.perspective = perspective;
            cost.healthcareCost = this->multiplyMatrix(
                this->history.stateHistory,
                this->costLoader.getHealthcareUtilizationCost(perspective));
            cost.pharmaCost = this->multiplyMatrix(
                this->history.stateHistory,
                this->costLoader.getPharmaceuticalCost(perspective));

            cost.treatmentCost = this->multiplyMatrix(
                this->history.stateHistory,
                this->costLoader.getTreatmentUtilizationCost(perspective));
            cost.nonFatalOverdoseCost = this->multiplyDouble(
                this->history.overdoseHistory,
                this->costLoader.getNonFatalOverdoseCost(perspective));

            cost.fatalOverdoseCost = this->multiplyDouble(
                this->history.fatalOverdoseHistory,
                this->costLoader.getFatalOverdoseCost(perspective));
            costs.push_back(cost);
        }
        return costs;
    }

    Data::Utility PostSimulationCalculator::calculateUtility() const {
        Data::Utility util;
        util.backgroundUtility =
            this->multiplyMatrix(this->history.stateHistory,
                                 this->utilityLoader.getBackgroundUtility());
        util.oudUtility = this->multiplyMatrix(
            this->history.stateHistory, this->utilityLoader.getOUDUtility());
        util.settingUtility =
            this->multiplyMatrix(this->history.stateHistory,
                                 this->utilityLoader.getSettingUtility());
        return util;
    }

    Data::Matrix3dOverTime PostSimulationCalculator::multiplyDouble(
        Data::Matrix3dOverTime const &overdose, double const &value) const {
        std::vector<Data::Matrix3d> overdoseVec = overdose.getMatrices();
        std::vector<Data::Matrix3d> result;

        for (Data::Matrix3d timeMat : overdoseVec) {
            Data::Matrix3d valueMatrix =
                Utilities::Matrix3dFactory::Create(
                    timeMat.dimension(Data::OUD),
                    timeMat.dimension(Data::INTERVENTION),
                    timeMat.dimension(Data::DEMOGRAPHIC_COMBO))
                    .constant(value);

            Data::Matrix3d temp = timeMat * valueMatrix;
            result.push_back(temp);
        }

        Data::Matrix3dOverTime ret(result);
        return ret;
    }

    Data::Matrix3dOverTime PostSimulationCalculator::multiplyMatrix(
        Data::Matrix3dOverTime const &state,
        Data::Matrix3d const &value) const {
        std::vector<Data::Matrix3d> result;
        std::vector<Data::Matrix3d> stateVec = state.getMatrices();
        for (Data::Matrix3d timeMat : stateVec) {
            Data::Matrix3d temp = timeMat * value;
            result.push_back(temp);
        }

        Data::Matrix3dOverTime ret(result);
        return ret;
    }
} // namespace Calculator