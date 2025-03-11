////////////////////////////////////////////////////////////////////////////////
// File: cost_loader_internals.hpp                                            //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-11                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_COSTLOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_COSTLOADERINTERNALS_HPP_

#include <respond/data_ops/cost_loader.hpp>

#include "base_loader_internals.hpp"

namespace respond::data_ops {
    class CostLoaderImpl : public virtual CostLoader, public BaseLoader {
    public:
        CostLoaderImpl(const std::string &directory = "",
                       const std::string &log_name = "console")
            : BaseLoader(directory, log_name) {}

        ~CostLoaderImpl() = default;

        std::unordered_map<std::string, Matrix3d>
        LoadHealthcareUtilizationCost(const std::string &file) override;

        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        LoadOverdoseCost(const std::string &file) override;

        std::unordered_map<std::string, Matrix3d>
        LoadPharmaceuticalCost(const std::string &file) override;

        std::unordered_map<std::string, Matrix3d>
        LoadTreatmentUtilizationCost(const std::string &file) override;

        Matrix3d GetHealthcareUtilizationCost(
            const std::string &perspective) const override {
            if (this->healthcare_utilization_cost.find(perspective) !=
                this->healthcare_utilization_cost.end()) {
                return this->healthcare_utilization_cost.at(perspective);
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
            if (this->pharmaceutical_cost.find(perspective) !=
                this->pharmaceutical_cost.end()) {
                return this->pharmaceutical_cost.at(perspective);
            }
            respond::utils::LogWarning(logger_name,
                                       "Pharmaceutical Cost not found for "
                                       "perspective: " +
                                           perspective);
            Matrix3d result;
            return result;
        }

        Matrix3d GetTreatmentUtilizationCost(
            const std::string &perspective) const override {
            if (this->treatment_utilization_cost.find(perspective) !=
                this->treatment_utilization_cost.end()) {
                return this->treatment_utilization_cost.at(perspective);
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

        double
        GetFatalOverdoseCost(const std::string &perspective) const override;

    private:
        std::unordered_map<std::string, Matrix3d> healthcare_utilization_cost;
        std::unordered_map<std::string, Matrix3d> pharmaceutical_cost;
        std::unordered_map<std::string, Matrix3d> treatment_utilization_cost;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            overdose_costs_map;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            pharmaceutical_costs_map;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            treatment_utilization_cost_map;

        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        LoadTreatmentUtilizationCostMap(Data::IDataTablePtr table);
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        LoadPharmaceuticalCostMap(Data::IDataTablePtr table);

        void LoadCostViaPerspective(
            std::unordered_map<std::string, Matrix3d> &cost,
            std::unordered_map<std::string,
                               std::unordered_map<std::string, double>>
                &cost_map);
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_COSTLOADERINTERNALS_HPP_