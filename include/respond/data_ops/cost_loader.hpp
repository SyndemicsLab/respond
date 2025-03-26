////////////////////////////////////////////////////////////////////////////////
// File: cost_loader.hpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_COSTLOADER_HPP_
#define RESPOND_DATAOPS_COSTLOADER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <respond/data_ops/matrices.hpp>

namespace respond::data_ops {
class CostLoader {
public:
    virtual ~CostLoader() = default;

    virtual std::unordered_map<std::string, Matrix3d>
    LoadHealthcareUtilizationCost(const std::string &file) = 0;

    virtual std::unordered_map<std::string,
                               std::unordered_map<std::string, double>>
    LoadOverdoseCost(const std::string &file) = 0;

    virtual std::unordered_map<std::string, Matrix3d>
    LoadPharmaceuticalCost(const std::string &file) = 0;

    virtual std::unordered_map<std::string, Matrix3d>
    LoadTreatmentUtilizationCost(const std::string &file) = 0;

    virtual Matrix3d
    GetHealthcareUtilizationCost(const std::string &perspective) const = 0;

    virtual Matrix3d
    GetPharmaceuticalCost(const std::string &perspective) const = 0;

    virtual Matrix3d
    GetTreatmentUtilizationCost(const std::string &perspective) const = 0;

    virtual double
    GetNonFatalOverdoseCost(const std::string &perspective) const = 0;

    virtual double
    GetFatalOverdoseCost(const std::string &perspective) const = 0;

    static std::unique_ptr<CostLoader>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_COSTLOADER_HPP_