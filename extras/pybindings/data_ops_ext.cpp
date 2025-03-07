////////////////////////////////////////////////////////////////////////////////
// File: data_ops_ext.cpp                                                     //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/data_ops/CostLoader.hpp>
#include <respond/data_ops/DataFormatter.hpp>
#include <respond/data_ops/DataLoader.hpp>
#include <respond/data_ops/DataTypes.hpp>
#include <respond/data_ops/UtilityLoader.hpp>
#include <respond/data_ops/Writer.hpp>

#include <memory>
#include <pybind11/eigen.h>
#include <pybind11/eigen/tensor.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace data_ops;

PYBIND11_MODULE(data_ops, m) {
    py::class_<IBaseLoader, std::shared_ptr<IBaseLoader>>(m, "IBaseLoader");

    py::class_<BaseLoader, std::shared_ptr<BaseLoader>, IBaseLoader>(
        m, "BaseLoader")
        .def("loadConfigFile", &BaseLoader::loadConfigFile)
        .def("getDuration", &BaseLoader::getDuration)
        .def("getAgingInterval", &BaseLoader::getAgingInterval)
        .def("getInterventionChangeTimes",
             &BaseLoader::getInterventionChangeTimes)
        .def("getEnteringSampleChangeTimes",
             &BaseLoader::getEnteringSampleChangeTimes)
        .def("getOverdoseChangeTimes", &BaseLoader::getOverdoseChangeTimes)
        .def("getInterventions", &BaseLoader::getInterventions)
        .def("setInterventions", &BaseLoader::setInterventions)
        .def("getOUDStates", &BaseLoader::getOUDStates)
        .def("setOUDStates", &BaseLoader::setOUDStates)
        .def("getDemographics", &BaseLoader::getDemographics)
        .def("setDemographics", &BaseLoader::setDemographics)
        .def("getDemographicCombos", &BaseLoader::getDemographicCombos)
        .def("setDemographicCombos", &BaseLoader::setDemographicCombos)
        .def("getGeneralStatsOutputTimesteps",
             &BaseLoader::getGeneralStatsOutputTimesteps)
        .def("getGeneralOutputsSwitch", &BaseLoader::getGeneralOutputsSwitch);

    py::class_<IDataLoader, std::shared_ptr<IDataLoader>>(m, "IDataLoader");

    py::class_<DataLoader, std::shared_ptr<DataLoader>, BaseLoader,
               IDataLoader>(m, "DataLoader")
        .def(py::init<Data::IConfigablePtr &, std::string const &,
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
        .def("loadMortalityRates", &DataLoader::loadMortalityRates)
        .def("loadOverdoseRates", &DataLoader::loadOverdoseRates)
        .def("loadFatalOverdoseRates", &DataLoader::loadFatalOverdoseRates)
        .def("loadInterventionInitRates",
             &DataLoader::loadInterventionInitRates);

    py::class_<ICostLoader, std::shared_ptr<ICostLoader>>(m, "ICostLoader");

    py::class_<CostLoader, std::shared_ptr<CostLoader>, BaseLoader,
               ICostLoader>(m, "CostLoader")
        .def(py::init<Data::IConfigablePtr &, std::string const &,
                      std::shared_ptr<spdlog::logger>>())
        .def(py::init<std::string const &, std::shared_ptr<spdlog::logger>>())
        .def(py::init<>())
        .def("loadHealthcareUtilizationCost",
             &CostLoader::loadHealthcareUtilizationCost)
        .def("loadOverdoseCost", &CostLoader::loadOverdoseCost)
        .def("loadPharmaceuticalCost", &CostLoader::loadPharmaceuticalCost)
        .def("loadTreatmentUtilizationCost",
             &CostLoader::loadTreatmentUtilizationCost)
        .def("getHealthcareUtilizationCost",
             &CostLoader::getHealthcareUtilizationCost)
        .def("getPharmaceuticalCost", &CostLoader::getPharmaceuticalCost)
        .def("getTreatmentUtilizationCost",
             &CostLoader::getTreatmentUtilizationCost)
        .def("getNonFatalOverdoseCost", &CostLoader::getNonFatalOverdoseCost)
        .def("getFatalOverdoseCost", &CostLoader::getFatalOverdoseCost)
        .def("getCostSwitch", &CostLoader::getCostSwitch)
        .def("getDiscountRate", &CostLoader::getDiscountRate);

    py::class_<IUtilityLoader, std::shared_ptr<IUtilityLoader>>(
        m, "IUtilityLoader");

    py::class_<UtilityLoader, std::shared_ptr<UtilityLoader>, BaseLoader,
               IUtilityLoader>(m, "UtilityLoader")
        .def(py::init<Data::IConfigablePtr &, std::string const &,
                      std::shared_ptr<spdlog::logger>>())
        .def(py::init<std::string const &, std::shared_ptr<spdlog::logger>>())
        .def(py::init<>())
        .def("loadBackgroundUtility", &UtilityLoader::loadBackgroundUtility)
        .def("loadOUDUtility", &UtilityLoader::loadOUDUtility)
        .def("loadSettingUtility", &UtilityLoader::loadSettingUtility)
        .def("getBackgroundUtility", &UtilityLoader::getBackgroundUtility)
        .def("getOUDUtility", &UtilityLoader::getOUDUtility)
        .def("getSettingUtility", &UtilityLoader::getSettingUtility);

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

    py::class_<Writer, std::shared_ptr<Writer>>(m, "Writer")
        .def(py::init<>())
        .def(py::init<std::string, std::vector<int>, WriteType>())
        .def("setWriteType", &Writer::setWriteType)
        .def("getWriteType", &Writer::getWriteType)
        .def("setDirname", &Writer::setDirname)
        .def("getDirname", &Writer::getDirname);

    py::class_<InputWriter, std::shared_ptr<InputWriter>, Writer>(m,
                                                                  "InputWriter")
        .def(py::init<std::string, std::vector<int>, WriteType>())
        .def(py::init<>())
        .def("writeParameters", &InputWriter::writeParameters);

    py::class_<Cost>(m, "Cost")
        .def(py::init<>())
        .def_readwrite("perspective", &Cost::perspective)
        .def_readwrite("healthcareCost", &Cost::healthcareCost)
        .def_readwrite("nonFatalOverdoseCost", &Cost::nonFatalOverdoseCost)
        .def_readwrite("fatalOverdoseCost", &Cost::fatalOverdoseCost)
        .def_readwrite("pharmaCost", &Cost::pharmaCost)
        .def_readwrite("treatmentCost", &Cost::treatmentCost);

    py::class_<DataFormatter>(m, "DataFormatter")
        .def(py::init<>())
        .def("extractTimesteps", &DataFormatter::extractTimesteps,
             py::arg("timesteps"), py::arg("history"),
             py::arg("costs") = py::none(), py::arg("utilities") = py::none(),
             py::arg("costSwitch") = false);

    py::enum_<WriteType>(m, "WriteType")
        .value("FILE", WriteType::FILE)
        .value("STRING", WriteType::STRING)
        .export_values();
}