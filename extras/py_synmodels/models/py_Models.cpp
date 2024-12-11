#include "models/Respond.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace synmodels::models;

void init_models(py::module &m) {
    py::class_<Respond>(m, "Respond")
        .def(py::init(&Respond::Create))
        .def("Run", &Respond::Run)
        .def("GetState", &Respond::GetState)
        .def("GetSimulationHistory", &Respond::GetSimulationHistory)
        .def_static("Create", &Respond::Create);
}