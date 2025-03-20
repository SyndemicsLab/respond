////////////////////////////////////////////////////////////////////////////////
// File: data_ops_ext.cpp                                                     //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/respond.hpp>

#include <memory>
#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace respond::data_ops;
using namespace respond::model;

PYBIND11_MODULE(respond, m) {
    auto data_ops = m.def_submodule(
        "data_ops",
        "A submodule containing the data operations necessary for RESPOND.");

    py::class_<DataLoader>(data_ops, "DataLoader")
        .def(py::init<const std::string &, const std::string &>())
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
        .def("LoadEnteringSamples", &DataLoader::LoadEnteringSamples)
        .def("LoadOUDTransitionRates", &DataLoader::LoadOUDTransitionRates)
        .def("LoadInterventionInitRates",
             &DataLoader::LoadInterventionInitRates)
        .def("LoadInterventionTransitionRates",
             &DataLoader::LoadInterventionTransitionRates)
        .def("LoadOverdoseRates", &DataLoader::LoadOverdoseRates)
        .def("LoadFatalOverdoseRates", &DataLoader::LoadFatalOverdoseRates)
        .def("LoadMortalityRates", &DataLoader::LoadMortalityRates)
        .def("GetConfig", &DataLoader::GetConfig);

    py::class_<CostLoader>(data_ops, "CostLoader")
        .def(py::init<const std::string &, const std::string &>())
        .def("GetCosts", &CostLoader::GetCosts);

    py::class_<UtilityLoader>();

    auto model = m.def_submodule(
        "model", "A submodule containing the model components for RESPOND.");
}