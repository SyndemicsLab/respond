#include "data/py_Data.cpp"
#include "kernels/py_Kernels.cpp"
#include "models/py_Models.cpp"

#include <Eigen/Eigen>
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(SynModels, m) {
    py::module data = m.def_submodule("Data", "Data docstring");
    init_data(data);

    py::module kernels = m.def_submodule("Kernels", "Kernels docstring");
    init_kernels(kernels);

    py::module models = m.def_submodule("Models", "Models docstring");
    init_models(models);
}