#include "DataLoader.hpp"
#include <DataTypes.hpp>
#include <memory>
#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace Matrixify;

PYBIND11_MODULE(Matrixify, m) {
    py::class_<Loader, std::shared_ptr<Loader>>(m, "Loader")
        .def("loadConfigurationFile", &Loader::loadConfigurationFile)
        .def("getDuration", &Loader::getDuration)
        .def("getAgingInterval", &Loader::getAgingInterval)
        .def("getInterventionChangeTimes", &Loader::getInterventionChangeTimes)
        .def("getEnteringSampleChangeTimes",
             &Loader::getEnteringSampleChangeTimes)
        .def("getOverdoseChangeTimes", &Loader::getOverdoseChangeTimes)
        .def("getInterventions", &Loader::getInterventions)
        .def("getOUDStates", &Loader::getOUDStates)
        .def("getDemographics", &Loader::getDemographics)
        .def("getDemographicCombos", &Loader::getDemographicCombos);

    py::class_<IDataLoader, std::shared_ptr<IDataLoader>>(m, "IDataLoader");

    py::class_<DataLoader, std::shared_ptr<DataLoader>, Loader, IDataLoader>(
        m, "DataLoader")
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
        .def("setInterventionInitRates", &DataLoader::setInterventionInitRates)
        .def("loadMortalityRates", &DataLoader::loadMortalityRates);

    py::class_<Matrix4d>(m, "Matrix4d")
        .def(py::init<std::vector<Matrix3d>>())
        .def(py::init<std::vector<Matrix3d>, std::vector<int>>())
        .def(py::init<>())
        .def("getMatrices", &Matrix4d::getMatrices);

    py::class_<History>(m, "History")
        .def_readwrite("stateHistory", &History::stateHistory)
        .def_readwrite("overdoseHistory", &History::overdoseHistory)
        .def_readwrite("fatalOverdoseHistory", &History::fatalOverdoseHistory)
        .def_readwrite("mortalityHistory", &History::mortalityHistory)
        .def_readwrite("interventionAdmissionHistory",
                       &History::interventionAdmissionHistory);
}