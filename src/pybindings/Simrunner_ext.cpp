#include "Simulation.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace Simulation;

PYBIND11_MODULE(Simulation, m) {
    py::class_<Sim>(m, "Sim")
        .def(py::init<std::shared_ptr<Matrixify::IDataLoader>>())
        .def("Run", &Sim::Run)
        .def("getHistory", &Sim::getHistory);
}