////////////////////////////////////////////////////////////////////////////////
// File: model_ext.cpp                                                        //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/model/Simulation.hpp>

#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace Simulation;

PYBIND11_MODULE(model, m) {
    py::class_<Respond>(m, "Respond")
        .def(py::init<std::shared_ptr<data_ops::IDataLoader>>())
        .def(
            "Run", &Respond::run,
            py::arg("dataloader") =
                static_cast<std::shared_ptr<data_ops::IDataLoader>>(nullptr),
            py::arg("costloader") =
                static_cast<std::shared_ptr<data_ops::ICostLoader>>(nullptr),
            py::arg("utilloader") =
                static_cast<std::shared_ptr<data_ops::IUtilityLoader>>(nullptr))
        .def("GetHistory", &Respond::getHistory)
        .def("AgeUp", &Respond::ageUp)
        .def("SetData", &Respond::setData)
        .def("SetCost", &Respond::setCost)
        .def("SetUtility", &Respond::setUtility)
        .def("SetLogger", &Respond::setLogger)
        .def("SetDuration", &Respond::setDuration)
        .def("GetCurrentTime", &Respond::getCurrentTime)
        .def("GetDuration", &Respond::getDuration)
        .def("GetState", &Respond::getState)
        .def("GetDataLoader", &Respond::getDataLoader)
        .def("GetCostLoader", &Respond::getCostLoader)
        .def("GetUtilityLoader", &Respond::getUtilityLoader)
        .def("GetLogger", &Respond::getLogger);
}