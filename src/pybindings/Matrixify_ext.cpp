#include "DataLoader.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace Matrixify;

PYBIND11_MODULE(Matrixify, m) {
    py::class_<DataLoader>(m, "DataLoader")
        .def(py::init<Data::IConfigurationPtr &, std::string const &,
                      std::shared_ptr<spdlog::logger>>())
        .def(py::init<std::string const &, std::shared_ptr<spdlog::logger>>())
        .def(py::init<>())
        .def("getInitialSample", &DataLoader::getInitialSample)
        .def("getEnteringSamples", &DataLoader::getEnteringSamples)
        .def("getOUDTransitionRates", &DataLoader::getOUDTransitionRates)
        .def("getInterventionTransitionRates",
             &DataLoader::getInterventionTransitionRates)
        .def("getFatalOverdoseRates", &DataLoader::getFatalOverdoseRates)
        .def("getMortalityRates", &DataLoader::getMortalityRates)
        .def("getInterventionInitRates", &DataLoader::getInterventionInitRates)
        .def("setInitialSample", &DataLoader::setInitialSample)
        .def("setEnteringSamples", &DataLoader::setEnteringSamples)
        .def("setOUDTransitionRates", &DataLoader::setOUDTransitionRates)
        .def("setInterventionTransitionRates",
             &DataLoader::setInterventionTransitionRates)
        .def("setOverdoseRates", &DataLoader::setOverdoseRates)
        .def("setFatalOverdoseRates", &DataLoader::setFatalOverdoseRates)
        .def("setMortalityRates", &DataLoader::setMortalityRates)
        .def("setInterventionInitRates", &DataLoader::setInterventionInitRates);
}