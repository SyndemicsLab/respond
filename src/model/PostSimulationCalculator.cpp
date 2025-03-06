////////////////////////////////////////////////////////////////////////////////
// File: PostSimulationCalculator.cpp                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respondsimulation/model/PostSimulationCalculator.hpp>

namespace Calculator {

    PostSimulationCalculator::PostSimulationCalculator(
        data_ops::History const &history) {
        this->history = history;
    }

    data_ops::CostList PostSimulationCalculator::calculateCosts(
        std::shared_ptr<data_ops::ICostLoader> const costLoader,
        bool discount) const {
        data_ops::CostList costs;
        std::vector<std::string> perspectives =
            costLoader->getCostPerspectives();

        for (std::string perspective : perspectives) {
            data_ops::Cost cost;
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

    data_ops::Matrix4d PostSimulationCalculator::calculateUtilities(
        std::shared_ptr<data_ops::IUtilityLoader> const utilityLoader,
        UTILITY_TYPE utilType, bool discount) const {
        data_ops::Matrix4d utilities;

        std::string perspective = "utility";

        data_ops::Matrix3d util;

        std::vector<data_ops::Matrix3d> utilityMatrices;
        utilityMatrices.push_back(
            utilityLoader->getBackgroundUtility(perspective));
        utilityMatrices.push_back(utilityLoader->getOUDUtility(perspective));
        utilityMatrices.push_back(
            utilityLoader->getSettingUtility(perspective));

        if (utilType == UTILITY_TYPE::MULT) {
            util = data_ops::vecMult(utilityMatrices);
        } else {
            util = data_ops::vecMin(utilityMatrices);
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
    PostSimulationCalculator::calculateLifeYears(bool provideDiscount,
                                                 double discountRate) const {
        std::vector<data_ops::Matrix3d> stateVec =
            this->history.stateHistory.getMatrices();
        if (stateVec.size() <= 0) {
            // log no state vector
            return 0.0;
        }

        data_ops::Matrix3d runningSum(stateVec[0].dimensions());
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
        data_ops::Matrix4d const data) const {
        std::vector<data_ops::Matrix3d> vec = data.getMatrices();
        if (vec.size() <= 0) {
            // log no vector
            return 0.0;
        }

        data_ops::Matrix3d runningSum(vec[0].dimensions());
        runningSum = runningSum.setZero();
        for (data_ops::Matrix3d mat : vec) {
            runningSum += mat;
        }

        Eigen::Tensor<double, 0> result = runningSum.sum();
        return result(0);
    }

    data_ops::Matrix4d PostSimulationCalculator::multiplyDouble(
        data_ops::Matrix4d const &overdose, double const &value,
        bool provideDiscount, double discountRate) const {
        std::vector<data_ops::Matrix3d> overdoseVec = overdose.getMatrices();
        std::vector<data_ops::Matrix3d> result;

        for (int t = 0; t < overdoseVec.size(); t++) {
            data_ops::Matrix3d valueMatrix =
                data_ops::Matrix3dFactory::Create(
                    overdoseVec[t].dimension(data_ops::OUD),
                    overdoseVec[t].dimension(data_ops::INTERVENTION),
                    overdoseVec[t].dimension(data_ops::DEMOGRAPHIC_COMBO))
                    .constant(value);
            data_ops::Matrix3d temp = overdoseVec[t] * valueMatrix;
            if (provideDiscount) {
                temp = this->provideDiscount(temp, discountRate, t);
            }
            result.push_back(temp);
        }

        data_ops::Matrix4d ret(result);
        return ret;
    }

    data_ops::Matrix4d PostSimulationCalculator::multiplyMatrix(
        data_ops::Matrix4d const &state, data_ops::Matrix3d const &value,
        bool provideDiscount, double discountRate) const {
        std::vector<data_ops::Matrix3d> result;
        std::vector<data_ops::Matrix3d> stateVec = state.getMatrices();
        for (int t = 0; t < stateVec.size(); t++) {
            data_ops::Matrix3d temp = stateVec[t] * value;
            if (provideDiscount) {
                temp = this->provideDiscount(temp, discountRate, t);
            }
            result.push_back(temp);
        }

        data_ops::Matrix4d ret(result);
        return ret;
    }

    data_ops::Matrix3d PostSimulationCalculator::provideDiscount(
        data_ops::Matrix3d data, double discountRate, int N, bool isDiscrete,
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

        data_ops::Matrix3d discount =
            data_ops::Matrix3dFactory::Create(
                data.dimension(data_ops::OUD),
                data.dimension(data_ops::INTERVENTION),
                data.dimension(data_ops::DEMOGRAPHIC_COMBO))
                .setConstant(discountConstant);
        data_ops::Matrix3d result = data - discount;
        return result;
    }
} // namespace Calculator