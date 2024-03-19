#include "Simulation.hpp"
#include <boost/python.hpp>

using namespace boost::python;
using namespace Simulation;

BOOST_PYTHON_MODULE(sim) {
    class_<Sim>("Sim", init<std::shared_ptr<Matrixify::IDataLoader>>())
        .def("Run", &Sim::Run);
}