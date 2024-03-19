#include "DataLoader.hpp"
#include <boost/python.hpp>

using namespace boost::python;
using namespace Matrixify;

BOOST_PYTHON_MODULE(libMatrixify) {
    class_<DataLoader>("DataLoader",
                       init<Data::IConfigurationPtr &, std::string const &,
                            std::shared_ptr<spdlog::logger>>())
        .def(init<std::string const &, std::shared_ptr<spdlog::logger>>())
        .def(init<>())
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