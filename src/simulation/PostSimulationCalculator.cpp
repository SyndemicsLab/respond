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

    PostsimulationCalculator::PostsimulationCalculator(
        matrixify::History const &history) {
        this->history = history;
    }

    matrixify::CostList PostsimulationCalculator::calculateCosts(
        std::shared_ptr<matrixify::ICostLoader> const costLoader,
        bool discount) const {
        matrixify::CostList costs;
        std::vector<std::string> perspectives =
            costLoader->getCostPerspectives();

        for (std::string perspective : perspectives) {
            matrixify::Cost cost;
            cost.perspective = perspective;
            if (discount) {
                cost.healthcareCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader->getHealthcareUtilizationCost(perspective), true,
                    costLoader->getDiscountRate());
                cost.pharmaCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader->getPharmaceuticalCost(perspective), true,
                    costLoader->getDiscountRate());

                cost.treatmentCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader->getTreatmentUtilizationCost(perspective), true,
                    costLoader->getDiscountRate());
                cost.nonFatalOverdoseCost = this->multiplyDouble(
                    this->history.overdoseHistory,
                    costLoader->getNonFatalOverdoseCost(perspective), true,
                    costLoader->getDiscountRate());

                cost.fatalOverdoseCost = this->multiplyDouble(
                    this->history.fatalOverdoseHistory,
                    costLoader->getFatalOverdoseCost(perspective), true,
                    costLoader->getDiscountRate());
            } else {
                cost.healthcareCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader->getHealthcareUtilizationCost(perspective),
                    false);
                cost.pharmaCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader->getPharmaceuticalCost(perspective), false);

                cost.treatmentCost = this->multiplyMatrix(
                    this->history.stateHistory,
                    costLoader->getTreatmentUtilizationCost(perspective),
                    false);
                cost.nonFatalOverdoseCost = this->multiplyDouble(
                    this->history.overdoseHistory,
                    costLoader->getNonFatalOverdoseCost(perspective), false);

                cost.fatalOverdoseCost = this->multiplyDouble(
                    this->history.fatalOverdoseHistory,
                    costLoader->getFatalOverdoseCost(perspective), false);
            }
            costs.push_back(cost);
        }
        return costs;
    }

    matrixify::Matrix4d PostsimulationCalculator::calculateUtilities(
        std::shared_ptr<matrixify::IUtilityLoader> const utilityLoader,
        UTILITY_TYPE utilType, bool discount) const {
        matrixify::Matrix4d utilities;

        std::string perspective = "utility";

        matrixify::Matrix3d util;

        std::vector<matrixify::Matrix3d> utilityMatrices;
        utilityMatrices.push_back(
            utilityLoader->getBackgroundUtility(perspective));
        utilityMatrices.push_back(utilityLoader->getOUDUtility(perspective));
        utilityMatrices.push_back(
            utilityLoader->getSettingUtility(perspective));

        if (utilType == UTILITY_TYPE::MULT) {
            util = matrixify::vecMult(utilityMatrices);
        } else {
            util = matrixify::vecMin(utilityMatrices);
        }

        if (discount) {
            utilities =
                this->multiplyMatrix(this->history.stateHistory, util, true,
                                     utilityLoader->getDiscountRate());
        } else {
            utilities =
                this->multiplyMatrix(this->history.stateHistory, util, false);
        }

        return utilities;
    }

    double
    PostsimulationCalculator::calculateLifeYears(bool provideDiscount,
                                                 double discountRate) const {
        std::vector<matrixify::Matrix3d> stateVec =
            this->history.stateHistory.getMatrices();
        if (stateVec.size() <= 0) {
            // log no state vector
            return 0.0;
        }

        matrixify::Matrix3d runningSum(stateVec[0].dimensions());
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

    double PostsimulationCalculator::totalAcrossTimeAndDims(
        matrixify::Matrix4d const data) const {
        std::vector<matrixify::Matrix3d> vec = data.getMatrices();
        if (vec.size() <= 0) {
            // log no vector
            return 0.0;
        }

        matrixify::Matrix3d runningSum(vec[0].dimensions());
        runningSum = runningSum.setZero();
        for (matrixify::Matrix3d mat : vec) {
            runningSum += mat;
        }

        Eigen::Tensor<double, 0> result = runningSum.sum();
        return result(0);
    }

    matrixify::Matrix4d PostsimulationCalculator::multiplyDouble(
        matrixify::Matrix4d const &overdose, double const &value,
        bool provideDiscount, double discountRate) const {
        std::vector<matrixify::Matrix3d> overdoseVec = overdose.getMatrices();
        std::vector<matrixify::Matrix3d> result;

        for (int t = 0; t < overdoseVec.size(); t++) {
            matrixify::Matrix3d valueMatrix =
                matrixify::Matrix3dFactory::Create(
                    overdoseVec[t].dimension(matrixify::OUD),
                    overdoseVec[t].dimension(matrixify::INTERVENTION),
                    overdoseVec[t].dimension(matrixify::DEMOGRAPHIC_COMBO))
                    .constant(value);
            matrixify::Matrix3d temp = overdoseVec[t] * valueMatrix;
            if (provideDiscount) {
                temp = this->provideDiscount(temp, discountRate, t);
            }
            result.push_back(temp);
        }

        matrixify::Matrix4d ret(result);
        return ret;
    }

    matrixify::Matrix4d PostsimulationCalculator::multiplyMatrix(
        matrixify::Matrix4d const &state, matrixify::Matrix3d const &value,
        bool provideDiscount, double discountRate) const {
        std::vector<matrixify::Matrix3d> result;
        std::vector<matrixify::Matrix3d> stateVec = state.getMatrices();
        for (int t = 0; t < stateVec.size(); t++) {
            matrixify::Matrix3d temp = stateVec[t] * value;
            if (provideDiscount) {
                temp = this->provideDiscount(temp, discountRate, t);
            }
            result.push_back(temp);
        }

        matrixify::Matrix4d ret(result);
        return ret;
    }

    matrixify::Matrix3d PostsimulationCalculator::provideDiscount(
        matrixify::Matrix3d data, double discountRate, int N, bool isDiscrete,
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

        matrixify::Matrix3d discount =
            matrixify::Matrix3dFactory::Create(
                data.dimension(matrixify::OUD),
                data.dimension(matrixify::INTERVENTION),
                data.dimension(matrixify::DEMOGRAPHIC_COMBO))
                .setConstant(discountConstant);
        matrixify::Matrix3d result = data - discount;
        return result;
    }
} // namespace Calculator