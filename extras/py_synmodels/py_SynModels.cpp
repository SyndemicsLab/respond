#include "data/py_Data.hpp"
#include "kernels/py_Kernels.hpp"
#include "models/py_Models.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(SynModels, m) {
    init_data(m);
    init_kernels(m);
    init_models(m);
}