////////////////////////////////////////////////////////////////////////////////
// File: cost_loader.hpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPONDSIMULATION_DATAOPS_COSTLOADER_HPP_
#define RESPONDSIMULATION_DATAOPS_COSTLOADER_HPP_

#include <string>
#include <unordered_map>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/base_loader.hpp>
#include <respond/data_ops/data_types.hpp>

namespace respond::data_ops {

    // using ICostLoaderPtr = std::shared_ptr<ICostLoader>;

    class CostLoader : public virtual BaseLoader {
    public:
        // CostLoader(Data::IConfigablePtr config, std::string const &inputDir,
        //            std::shared_ptr<spdlog::logger> logger);

        // delegating constructors
        // CostLoader() : CostLoader(nullptr, "", nullptr) {}
        // CostLoader(Data::IConfigablePtr config)
        //     : CostLoader(config, "", nullptr) {}
        // CostLoader(Data::IConfigablePtr config, std::string const &inputDir)
        //     : CostLoader(config, inputDir, nullptr) {}
        // CostLoader(std::string const &inputDir,
        //            std::shared_ptr<spdlog::logger> logger)
        //     : CostLoader(nullptr, inputDir, logger) {}
        // CostLoader(std::string const &inputDir)
        //     : CostLoader(nullptr, inputDir, nullptr) {}

        virtual ~CostLoader() = default;

        virtual std::unordered_map<std::string, Matrix3d>
        LoadHealthcareUtilizationCost(std::string const &csvName) = 0;

        virtual std::unordered_map<std::string,
                                   std::unordered_map<std::string, double>>
        LoadOverdoseCost(std::string const &csvName) = 0;

        virtual std::unordered_map<std::string, Matrix3d>
        LoadPharmaceuticalCost(std::string const &csvName) = 0;

        virtual std::unordered_map<std::string, Matrix3d>
        LoadTreatmentUtilizationCost(std::string const &csvName) = 0;

        virtual Matrix3d
        GetHealthcareUtilizationCost(std::string const &perspective) const = 0;
        // {
        //     if (this->healthcareUtilizationCost.find(perspective) !=
        //         this->healthcareUtilizationCost.end()) {
        //         return this->healthcareUtilizationCost.at(perspective);
        //     }
        //     // add warning
        //     Matrix3d result;
        //     return result;
        // }

        virtual Matrix3d
        GetPharmaceuticalCost(std::string const &perspective) const = 0;
        // {
        //     if (this->pharmaceuticalCost.find(perspective) !=
        //         this->pharmaceuticalCost.end()) {
        //         return this->pharmaceuticalCost.at(perspective);
        //     }
        //     // add warning
        //     Matrix3d result;
        //     return result;
        // }

        virtual Matrix3d
        GetTreatmentUtilizationCost(std::string const &perspective) const = 0;
        // {
        //     if (this->treatmentUtilizationCost.find(perspective) !=
        //         this->treatmentUtilizationCost.end()) {
        //         return this->treatmentUtilizationCost.at(perspective);
        //     }
        //     // add warning
        //     Matrix3d result;
        //     return result;
        // }

        virtual double
        GetNonFatalOverdoseCost(std::string const &perspective) const = 0;

        virtual double
        GetFatalOverdoseCost(std::string const &perspective) const = 0;

        virtual std::vector<std::string> GetCostPerspectives() const = 0;

        virtual bool GetCostSwitch() const = 0;

        virtual double GetDiscountRate() const = 0;
    };
} // namespace respond::data_ops

#endif // RESPONDSIMULATION_DATAOPS_COSTLOADER_HPP_