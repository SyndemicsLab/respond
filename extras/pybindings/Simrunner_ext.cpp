#include "Simulation.hpp"
#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace Simulation;

PYBIND11_MODULE(Simulation, m) {
    py::class_<Respond>(m, "Respond")
        .def(py::init<std::shared_ptr<Matrixify::IDataLoader>>())
        .def("RunRESPOND", &Respond::run,
             py::arg("dataloader") =
                 static_cast<std::shared_ptr<Matrixify::IDataLoader>>(nullptr),
             py::arg("costloader") =
                 static_cast<std::shared_ptr<Matrixify::ICostLoader>>(nullptr),
             py::arg("utilloader") =
                 static_cast<std::shared_ptr<Matrixify::IUtilityLoader>>(
                     nullptr))
        .def("getHistory", &Respond::getHistory)
        .def("ageUp", &Respond::ageUp)
        .def("setData", &Respond::setData)
        .def("setCost", &Respond::setCost)
        .def("setUtility", &Respond::setUtility)
        .def("setLogger", &Respond::setLogger)
        .def("setDuration", &Respond::setDuration)
        .def("getCurrentTime", &Respond::getCurrentTime)
        .def("getDuration", &Respond::getDuration)
        .def("getState", &Respond::getState)
        .def("getDataLoader", &Respond::getDataLoader)
        .def("getCostLoader", &Respond::getCostLoader)
        .def("getUtilityLoader", &Respond::getUtilityLoader)
        .def("getLogger", &Respond::getLogger);
}