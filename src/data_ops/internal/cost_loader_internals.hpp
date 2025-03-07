////////////////////////////////////////////////////////////////////////////////
// File: cost_loader_internals.hpp                                            //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_COSTLOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_COSTLOADERINTERNALS_HPP_

#include <respond/data_ops/cost_loader.hpp>

namespace respond::data_ops {
    class CostLoaderImpl : public virtual CostLoader {
    public:
        CostLoaderImpl(Data::IConfigablePtr config, std::string const &inputDir,
                       std::shared_ptr<spdlog::logger> logger);

        CostLoaderImpl() : CostLoaderImpl(nullptr, "", nullptr) {}
        CostLoaderImpl(Data::IConfigablePtr config)
            : CostLoaderImpl(config, "", nullptr) {}
        CostLoaderImpl(Data::IConfigablePtr config, std::string const &inputDir)
            : CostLoaderImpl(config, inputDir, nullptr) {}
        CostLoaderImpl(std::string const &inputDir,
                       std::shared_ptr<spdlog::logger> logger)
            : CostLoaderImpl(nullptr, inputDir, logger) {}
        CostLoaderImpl(std::string const &inputDir)
            : CostLoaderImpl(nullptr, inputDir, nullptr) {}

        ~CostLoaderImpl() = default;

        std::unordered_map<std::string, Matrix3d>
        LoadHealthcareUtilizationCost(std::string const &csvName) override;

        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        LoadOverdoseCost(std::string const &csvName) override;

        std::unordered_map<std::string, Matrix3d>
        LoadPharmaceuticalCost(std::string const &csvName) override;

        std::unordered_map<std::string, Matrix3d>
        LoadTreatmentUtilizationCost(std::string const &csvName) override;

        Matrix3d GetHealthcareUtilizationCost(
            std::string const &perspective) const override {
            if (this->healthcareUtilizationCost.find(perspective) !=
                this->healthcareUtilizationCost.end()) {
                return this->healthcareUtilizationCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        Matrix3d
        GetPharmaceuticalCost(std::string const &perspective) const override {
            if (this->pharmaceuticalCost.find(perspective) !=
                this->pharmaceuticalCost.end()) {
                return this->pharmaceuticalCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        Matrix3d GetTreatmentUtilizationCost(
            std::string const &perspective) const override {
            if (this->treatmentUtilizationCost.find(perspective) !=
                this->treatmentUtilizationCost.end()) {
                return this->treatmentUtilizationCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        double
        GetNonFatalOverdoseCost(std::string const &perspective) const override;

        double
        GetFatalOverdoseCost(std::string const &perspective) const override;

        std::vector<std::string> GetCostPerspectives() const override;

        bool GetCostSwitch() const override;

        double GetDiscountRate() const override;

    private:
        std::unordered_map<std::string, Matrix3d> healthcareUtilizationCost;
        std::unordered_map<std::string, Matrix3d> pharmaceuticalCost;
        std::unordered_map<std::string, Matrix3d> treatmentUtilizationCost;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            overdoseCostsMap;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            pharmaceuticalCostsMap;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            treatmentUtilizationCostMap;

        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        LoadTreatmentUtilizationCostMap(Data::IDataTablePtr table);
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        LoadPharmaceuticalCostMap(Data::IDataTablePtr table);

        void LoadCostViaPerspective(
            std::unordered_map<std::string, Matrix3d> &costParameter,
            std::unordered_map<std::string,
                               std::unordered_map<std::string, double>>
                &costParameterMap);
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_COSTLOADERINTERNALS_HPP_