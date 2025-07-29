////////////////////////////////////////////////////////////////////////////////
// File: cost_loader_internals.hpp                                            //
// Project: respond                                                           //
// Created Date: 2025-07-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_PREPROCESS_COSTLOADERINTERNALS_HPP_
#define RESPOND_PREPROCESS_COSTLOADERINTERNALS_HPP_

#include <respond/preprocess/cost_loader.hpp>

#include <memory>
#include <string>
#include <unordered_map>

#include <datamanagement/DataManagement.hpp>

#include <respond/utils/logging.hpp>

#include "base_loader_internals.hpp"

namespace respond {
namespace preprocess {
class CostLoaderImpl : public virtual CostLoader, public BaseLoaderImpl {
public:
    CostLoaderImpl(const std::string &logger_name = "console")
        : BaseLoaderImpl(logger_name) {}

    ~CostLoaderImpl() = default;

    StringUOMap<Matrix3d>
    LoadHealthcareUtilizationCost(const std::string &file) override;

    StringUOMap<StringUOMap<double>>
    LoadOverdoseCost(const std::string &file) override;

    StringUOMap<Matrix3d>
    LoadPharmaceuticalCost(const std::string &file) override;

    StringUOMap<Matrix3d>
    LoadTreatmentUtilizationCost(const std::string &file) override;

    Matrix3d GetHealthcareUtilizationCost(
        const std::string &perspective) const override {
        if (healthcare_utilization_cost.find(perspective) !=
            healthcare_utilization_cost.end()) {
            return healthcare_utilization_cost.at(perspective);
        }
        respond::utils::LogWarning(logger_name,
                                   "Healthcare Utilization Cost not found "
                                   "for perspective: " +
                                       perspective);
        Matrix3d result;
        return result;
    }

    Matrix3d
    GetPharmaceuticalCost(const std::string &perspective) const override {
        if (pharmaceutical_cost.find(perspective) !=
            pharmaceutical_cost.end()) {
            return pharmaceutical_cost.at(perspective);
        }
        respond::utils::LogWarning(logger_name,
                                   "Pharmaceutical Cost not found for "
                                   "perspective: " +
                                       perspective);
        Matrix3d result;
        return result;
    }

    Matrix3d
    GetTreatmentUtilizationCost(const std::string &perspective) const override {
        if (treatment_utilization_cost.find(perspective) !=
            treatment_utilization_cost.end()) {
            return treatment_utilization_cost.at(perspective);
        }
        respond::utils::LogWarning(logger_name,
                                   "Treatment Utilization Cost not found "
                                   "for perspective: " +
                                       perspective);
        Matrix3d result;
        return result;
    }

    double
    GetNonFatalOverdoseCost(const std::string &perspective) const override;

    double GetFatalOverdoseCost(const std::string &perspective) const override;

private:
    StringUOMap<Matrix3d> healthcare_utilization_cost;
    StringUOMap<Matrix3d> pharmaceutical_cost;
    StringUOMap<Matrix3d> treatment_utilization_cost;
    StringUOMap<StringUOMap<double>> overdose_costs_map;
    StringUOMap<StringUOMap<double>> pharmaceutical_costs_map;
    StringUOMap<StringUOMap<double>> treatment_utilization_cost_map;

    StringUOMap<StringUOMap<double>>
    LoadTreatmentUtilizationCostMap(Data::IDataTablePtr table);
    StringUOMap<StringUOMap<double>>
    LoadPharmaceuticalCostMap(Data::IDataTablePtr table);

    void LoadCostViaPerspective(StringUOMap<Matrix3d> &cost,
                                StringUOMap<StringUOMap<double>> &cost_map);
};
} // namespace preprocess
} // namespace respond

#endif // RESPOND_PREPROCESS_COSTLOADERINTERNALS_HPP_