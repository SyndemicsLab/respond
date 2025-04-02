////////////////////////////////////////////////////////////////////////////////
// File: data_ops_ext.cpp                                                     //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/respond.hpp>

#include <memory>
#include <string>

#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace respond::data_ops;
using namespace respond::model;

PYBIND11_MODULE(respondpy, m) {
    auto data_ops = m.def_submodule(
        "data_ops",
        "A submodule containing the data operations necessary for RESPOND.");

    // cost_loader.hpp
    py::class_<CostLoader>(data_ops, "CostLoader")
        .def(py::init(&CostLoader::Create), pybind11::arg("directory") = "",
             pybind11::arg("log_name") = "console")
        .def("LoadHealthcareUtilizationCost",
             &CostLoader::LoadHealthcareUtilizationCost)
        .def("LoadOverdoseCost", &CostLoader::LoadOverdoseCost)
        .def("LoadPharmaceuticalCost", &CostLoader::LoadPharmaceuticalCost)
        .def("LoadTreatmentUtilizationCost",
             &CostLoader::LoadTreatmentUtilizationCost)
        .def("GetHealthcareUtilizationCost",
             &CostLoader::GetHealthcareUtilizationCost)
        .def("GetPharmaceuticalCost", &CostLoader::GetPharmaceuticalCost)
        .def("GetTreatmentUtilizationCost",
             &CostLoader::GetTreatmentUtilizationCost)
        .def("GetNonFatalOverdoseCost", &CostLoader::GetNonFatalOverdoseCost)
        .def("GetFatalOverdoseCost", &CostLoader::GetFatalOverdoseCost);

    // data_formatter.hpp
    py::class_<DataFormatter>(data_ops, "DataFormatter")
        .def(py::init(&DataFormatter::Create))
        .def("ExtractTimesteps", &DataFormatter::ExtractTimesteps);

    // data_loader.hpp
    py::class_<DataLoader>(data_ops, "DataLoader")
        .def(py::init(&DataLoader::Create), pybind11::arg("directory") = "",
             pybind11::arg("log_name") = "console")
        .def("GetInitialSample", &DataLoader::GetInitialSample)
        .def("GetEnteringSamples", &DataLoader::GetEnteringSamples)
        .def("GetOUDTransitionRates", &DataLoader::GetOUDTransitionRates)
        .def("GetInterventionTransitionRates",
             &DataLoader::GetInterventionTransitionRates)
        .def("GetOverdoseRates", &DataLoader::GetOverdoseRates)
        .def("GetFatalOverdoseRates", &DataLoader::GetFatalOverdoseRates)
        .def("GetMortalityRates", &DataLoader::GetMortalityRates)
        .def("GetInterventionInitRates", &DataLoader::GetInterventionInitRates)
        .def("SetInitialSample", &DataLoader::SetInitialSample)
        .def("SetEnteringSamples", &DataLoader::SetEnteringSamples)
        .def("SetOUDTransitionRates", &DataLoader::SetOUDTransitionRates)
        .def("SetInterventionTransitionRates",
             &DataLoader::SetInterventionTransitionRates)
        .def("SetOverdoseRates", &DataLoader::SetOverdoseRates)
        .def("SetFatalOverdoseRates", &DataLoader::SetFatalOverdoseRates)
        .def("SetMortalityRates", &DataLoader::SetMortalityRates)
        .def("SetInterventionInitRates", &DataLoader::SetInterventionInitRates)
        .def("LoadInitialSample", &DataLoader::LoadInitialSample)
        .def("LoadEnteringSamples",
             py::overload_cast<const std::string &, const std::string &,
                               const std::string &>(
                 &DataLoader::LoadEnteringSamples))
        .def("LoadEnteringSamples", py::overload_cast<const std::string &>(
                                        &DataLoader::LoadEnteringSamples))
        .def("LoadOUDTransitionRates", &DataLoader::LoadOUDTransitionRates)
        .def("LoadInterventionInitRates",
             &DataLoader::LoadInterventionInitRates)
        .def("LoadInterventionTransitionRates",
             &DataLoader::LoadInterventionTransitionRates)
        .def("LoadOverdoseRates", &DataLoader::LoadOverdoseRates)
        .def("LoadFatalOverdoseRates", &DataLoader::LoadFatalOverdoseRates)
        .def("LoadMortalityRates", &DataLoader::LoadMortalityRates);

    // data_types.hpp
    py::enum_<Dimension>(data_ops, "Dimension")
        .value("kIntervention", Dimension::kIntervention)
        .value("kOud", Dimension::kOud)
        .value("kDemographicCombo", Dimension::kDemographicCombo)
        .export_values();

    py::class_<History>(data_ops, "History")
        .def(py::init())
        .def_readwrite("state_history", &History::state_history)
        .def_readwrite("overdose_history", &History::overdose_history)
        .def_readwrite("fatal_overdose_history",
                       &History::fatal_overdose_history)
        .def_readwrite("mortality_history", &History::mortality_history)
        .def_readwrite("intervention_admission_history",
                       &History::intervention_admission_history);

    py::class_<Cost>(data_ops, "Cost")
        .def(py::init())
        .def_readwrite("perspective", &Cost::perspective)
        .def_readwrite("healthcare_cost", &Cost::healthcare_cost)
        .def_readwrite("non_fatal_overdose_cost",
                       &Cost::non_fatal_overdose_cost)
        .def_readwrite("fatal_overdose_cost", &Cost::fatal_overdose_cost)
        .def_readwrite("pharma_cost", &Cost::pharma_cost)
        .def_readwrite("treatment_cost", &Cost::treatment_cost);

    py::enum_<UtilityType>(data_ops, "UtilityType")
        .value("kMin", UtilityType::kMin)
        .value("kMult", UtilityType::kMult)
        .export_values();

    py::class_<Totals>(data_ops, "Totals")
        .def(py::init())
        .def_readwrite("base_costs", &Totals::base_costs)
        .def_readwrite("disc_costs", &Totals::disc_costs)
        .def_readwrite("base_life_years", &Totals::base_life_years)
        .def_readwrite("disc_life_years", &Totals::disc_life_years)
        .def_readwrite("base_utility", &Totals::base_utility)
        .def_readwrite("disc_utility", &Totals::disc_utility);

    // matrices.hpp
    data_ops.def("CreateMatrix3d", &CreateMatrix3d,
                 "A Factory Function to generate a new Eigen Matrix3d.");
    data_ops.def("PrintMatrix3d", &PrintMatrix3d,
                 "Prints an Eigen Matrix3d to the provided stream.");
    data_ops.def("PrintTimedMatrix3d", &PrintTimedMatrix3d,
                 "Prints a TimedMatrix3d to the provided stream.");
    data_ops.def("Matrix3dVectorMinimum", &Matrix3dVectorMinimum,
                 "Returns the minimum of a vector of Eigen Matrix3d.");
    data_ops.def("Matrix3dVectorMultiplied", &Matrix3dVectorMultiplied,
                 "Returns the product of a vector of Eigen Matrix3d.");
    data_ops.def("TimedMatrix3dSummed", &TimedMatrix3dSummed,
                 "Returns the sum of a TimedMatrix3d.");
    data_ops.def("TimedMatrix3dSummedOverDimensions",
                 &TimedMatrix3dSummedOverDimensions,
                 "Returns the sum of all elements in a TimedMatrix3d.");
    data_ops.def("MultiplyTimedMatrix3dByDouble",
                 &MultiplyTimedMatrix3dByDouble,
                 "Multiply a TimedMatrix3d by a double.");
    data_ops.def("MultiplyTimedMatrix3dByMatrix",
                 &MultiplyTimedMatrix3dByMatrix,
                 "Multiply a TimedMatrix3d by another Matrix3d.");

    // utility_loader.hpp
    py::class_<UtilityLoader>(data_ops, "UtilityLoader")
        .def(py::init(&UtilityLoader::Create), pybind11::arg("directory") = "",
             pybind11::arg("log_name") = "console")
        .def("LoadBackgroundUtility", &UtilityLoader::LoadBackgroundUtility)
        .def("LoadOUDUtility", &UtilityLoader::LoadOUDUtility)
        .def("LoadSettingUtility", &UtilityLoader::LoadSettingUtility)
        .def("GetBackgroundUtility", &UtilityLoader::GetBackgroundUtility)
        .def("GetOUDUtility", &UtilityLoader::GetOUDUtility)
        .def("GetSettingUtility", &UtilityLoader::GetSettingUtility);

    // writer.hpp
    py::enum_<WriterType>(data_ops, "WriterType")
        .value("kInput", WriterType::kInput)
        .value("kOutput", WriterType::kOutput)
        .value("kHistory", WriterType::kHistory)
        .value("kCost", WriterType::kCost)
        .value("kUtilities", WriterType::kUtilities)
        .value("kTotals", WriterType::kTotals)
        .export_values();

    py::enum_<OutputType>(data_ops, "OutputType")
        .value("kString", OutputType::kString)
        .value("kFile", OutputType::kFile)
        .export_values();

    py::class_<Writer>(data_ops, "Writer")
        .def(py::init(&Writer::Create), pybind11::arg("directory") = "",
             pybind11::arg("log_name") = "console")
        .def("WriteInputData", &Writer::WriteInputData)
        .def("WriteHistoryData", &Writer::WriteHistoryData)
        .def("WriteCostData", &Writer::WriteCostData)
        .def("WriteUtilityData", &Writer::WriteUtilityData)
        .def("WriteTotalsData", &Writer::WriteTotalsData);

    auto model = m.def_submodule(
        "model", "A submodule containing the model components for RESPOND.");

    // post_sim.hpp
    model.def("CalculateDiscount", &CalculateDiscount,
              "Calculates the Discount for the provided Matrix3d.");
    model.def("CalculateCosts", &CalculateCosts,
              "Calculates the Costs of the provided History.");
    model.def("CalculateUtilities", &CalculateUtilities,
              "Calculate the Utilities of the provided History.");
    model.def("CalculateLifeYears", &CalculateLifeYears,
              "Calculate the Life Years of the provided History.");
    model.def("CalculateTotalCosts", &CalculateTotalCosts,
              "Calculate the Total Costs of the provided History.");

    // simulation.hpp
    py::class_<Respond>(model, "Respond")
        .def(py::init(&Respond::Create), pybind11::arg("log_name") = "console")
        .def("Run", &Respond::Run)
        .def("GetHistory", &Respond::GetHistory);
}