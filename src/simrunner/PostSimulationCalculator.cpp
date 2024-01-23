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
        Matrixify::History const &history) {
        this->history = history;
    }

    Matrixify::CostList PostSimulationCalculator::calculateCosts(
        Matrixify::ICostLoader const &costLoader, bool discount) const {
        Matrixify::CostList costs;
        std::vector<std::string> perspectives =
            costLoader.getCostPerspectives();

        for (std::string perspective : perspectives) {
            Matrixify::Cost cost;
            cost.perspective = perspective;
            if (discount) {
                cost.healthcareCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader.getHealthcareUtilizationCost(perspective), true,
                    costLoader.getDiscountRate());
                cost.pharmaCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader.getPharmaceuticalCost(perspective), true,
                    costLoader.getDiscountRate());

                cost.treatmentCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader.getTreatmentUtilizationCost(perspective), true,
                    costLoader.getDiscountRate());
                cost.nonFatalOverdoseCost = this->multiplyDouble(
                    this->history.overdoseHistory,
                    costLoader.getNonFatalOverdoseCost(perspective), true,
                    costLoader.getDiscountRate());

                cost.fatalOverdoseCost = this->multiplyDouble(
                    this->history.fatalOverdoseHistory,
                    costLoader.getFatalOverdoseCost(perspective), true,
                    costLoader.getDiscountRate());
            } else {
                cost.healthcareCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader.getHealthcareUtilizationCost(perspective),
                    false);
                cost.pharmaCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader.getPharmaceuticalCost(perspective), false);

                cost.treatmentCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader.getTreatmentUtilizationCost(perspective), false);
                cost.nonFatalOverdoseCost = this->multiplyDouble(
                    this->history.overdoseHistory,
                    costLoader.getNonFatalOverdoseCost(perspective), false);

                cost.fatalOverdoseCost = this->multiplyDouble(
                    this->history.fatalOverdoseHistory,
                    costLoader.getFatalOverdoseCost(perspective), false);
            }
            costs.push_back(cost);
        }
        return costs;
    }

    Matrixify::Matrix3dOverTime PostSimulationCalculator::calculateUtilities(
        Matrixify::IUtilityLoader const &utilityLoader, UTILITY_TYPE utilType,
        bool discount) const {
        Matrixify::Matrix3dOverTime utilities;

        std::string perspective = "utility";

        Matrixify::Matrix3d util;

        std::vector<Matrixify::Matrix3d> utilityMatrices;
        utilityMatrices.push_back(
            utilityLoader.getBackgroundUtility(perspective));
        utilityMatrices.push_back(utilityLoader.getOUDUtility(perspective));
        utilityMatrices.push_back(utilityLoader.getSettingUtility(perspective));

        if (utilType == UTILITY_TYPE::MULT) {
            util = Matrixify::vecMult(utilityMatrices);
        } else {
            util = Matrixify::vecMin(utilityMatrices);
        }

        if (discount) {
            utilities =
                this->multiplyMatrix(this->history.stateHistory, util, true,
                                     utilityLoader.getDiscountRate());
        } else {
            utilities =
                this->multiplyMatrix(this->history.stateHistory, util, false);
        }

        return utilities;
    }

    double
    PostSimulationCalculator::calculateLifeYears(bool provideDiscount,
                                                 double discountRate) const {
        std::vector<Matrixify::Matrix3d> stateVec =
            this->history.stateHistory.getMatrices();
        if (stateVec.size() <= 0) {
            // log no state vector
            return 0.0;
        }

        Matrixify::Matrix3d runningSum(stateVec[0].dimensions());
        runningSum = runningSum.setZero();

        for (int t = 0; t < stateVec.size(); ++t) {
            if (provideDiscount) {
                runningSum +=
                    this->provideDiscount(stateVec[t], discountRate, t);
            } else {
                runningSum += stateVec[t];
            }
        }

        Eigen::Tensor<double, 0> result = runningSum.sum();

        // dividing by 52 to switch from life weeks to life years
        return result(0) / 52.0;
    }

    double PostSimulationCalculator::totalAcrossTimeAndDims(
        Matrixify::Matrix3dOverTime const data) const {
        std::vector<Matrixify::Matrix3d> vec = data.getMatrices();
        if (vec.size() <= 0) {
            // log no vector
            return 0.0;
        }

        Matrixify::Matrix3d runningSum(vec[0].dimensions());
        runningSum = runningSum.setZero();
        for (Matrixify::Matrix3d mat : vec) {
            runningSum += mat;
        }

        Eigen::Tensor<double, 0> result = runningSum.sum();
        return result(0);
    }

    Matrixify::Matrix3dOverTime PostSimulationCalculator::multiplyDouble(
        Matrixify::Matrix3dOverTime const &overdose, double const &value,
        bool provideDiscount, double discountRate) const {
        std::vector<Matrixify::Matrix3d> overdoseVec = overdose.getMatrices();
        std::vector<Matrixify::Matrix3d> result;

        for (int t = 0; t < overdoseVec.size(); t++) {
            Matrixify::Matrix3d valueMatrix =
                Matrixify::Matrix3dFactory::Create(
                    overdoseVec[t].dimension(Matrixify::OUD),
                    overdoseVec[t].dimension(Matrixify::INTERVENTION),
                    overdoseVec[t].dimension(Matrixify::DEMOGRAPHIC_COMBO))
                    .constant(value);
            Matrixify::Matrix3d temp = overdoseVec[t] * valueMatrix;
            if (provideDiscount) {
                temp = this->provideDiscount(temp, discountRate, t);
            }
            result.push_back(temp);
        }

        Matrixify::Matrix3dOverTime ret(result);
        return ret;
    }

    Matrixify::Matrix3dOverTime PostSimulationCalculator::multiplyMatrix(
        Matrixify::Matrix3dOverTime const &state,
        Matrixify::Matrix3d const &value, bool provideDiscount,
        double discountRate) const {
        std::vector<Matrixify::Matrix3d> result;
        std::vector<Matrixify::Matrix3d> stateVec = state.getMatrices();
        for (int t = 0; t < stateVec.size(); t++) {
            Matrixify::Matrix3d temp = stateVec[t] * value;
            if (provideDiscount) {
                temp = this->provideDiscount(temp, discountRate, t);
            }
            result.push_back(temp);
        }

        Matrixify::Matrix3dOverTime ret(result);
        return ret;
    }

    Matrixify::Matrix3d PostSimulationCalculator::provideDiscount(
        Matrixify::Matrix3d data, double discountRate, int N, bool isDiscrete,
        bool weeklyTimestep) {
        if (!weeklyTimestep) {
            // notify we do not support this yet
            return data;
        }

        double discountConstant;
        if (isDiscrete) {
            discountConstant = 1 / pow((1.0 + (discountRate) / 52.0), N);

        } else {
            discountConstant = exp(-discountRate * (N / 52));
        }

        Matrixify::Matrix3d discount =
            Matrixify::Matrix3dFactory::Create(
                data.dimension(Matrixify::OUD),
                data.dimension(Matrixify::INTERVENTION),
                data.dimension(Matrixify::DEMOGRAPHIC_COMBO))
                .setConstant(discountConstant);
        Matrixify::Matrix3d result = data - discount;
        return result;
    }
} // namespace Calculator