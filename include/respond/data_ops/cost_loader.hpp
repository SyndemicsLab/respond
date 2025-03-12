////////////////////////////////////////////////////////////////////////////////
// File: cost_loader.hpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_COSTLOADER_HPP_
#define RESPOND_DATAOPS_COSTLOADER_HPP_

#include <string>
#include <unordered_map>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond::data_ops {
    class CostLoader {
    public:
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

        virtual Matrix3d
        GetPharmaceuticalCost(std::string const &perspective) const = 0;

        virtual Matrix3d
        GetTreatmentUtilizationCost(std::string const &perspective) const = 0;

        virtual double
        GetNonFatalOverdoseCost(std::string const &perspective) const = 0;

        virtual double
        GetFatalOverdoseCost(std::string const &perspective) const = 0;

        virtual std::vector<std::string> GetCostPerspectives() const = 0;

        virtual bool GetCostSwitch() const = 0;

        virtual double GetDiscountRate() const = 0;
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_COSTLOADER_HPP_