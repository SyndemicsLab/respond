#include "Simulation.hpp"
#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace Simulation;

PYBIND11_MODULE(Simulation, m) {
    py::class_<Sim>(m, "Sim")
        .def(py::init<std::shared_ptr<Matrixify::IDataLoader>>())
        .def("RunRESPOND", &Sim::Run)
        .def("getHistory", &Sim::getHistory)
        .def("GetEnteringSamples", &Sim::GetEnteringSamples)
        .def("GetOUDTransitions", &Sim::GetOUDTransitions)
        .def("GetInterventionTransitions", &Sim::GetInterventionTransitions)
        .def("GetOverdoseTransitions", &Sim::GetOverdoseTransitions)
        .def("GetFatalOverdoseTransitions", &Sim::GetFatalOverdoseTransitions)
        .def("GetMortalityTransitions", &Sim::GetMortalityTransitions);
}