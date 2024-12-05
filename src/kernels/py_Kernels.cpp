#include "StateTransitionModelImpl.hpp"
#include "kernels/StateTransitionModel.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace synmodels::kernels;

void init_kernels(py::module &m) {
    py::class_<StateTransitionModel, StateTransitionModelImpl>(
        m, "StateTransitionModel")
        .def(py::init<>())
        .def("SetState", &StateTransitionModel::SetState)
        .def("GetState", &StateTransitionModel::GetState)
        .def("SetTransitions", &StateTransitionModel::SetTransitions)
        .def("GetTransitions", &StateTransitionModel::GetTransitions)
        .def("Transition", &StateTransitionModel::Transition)
        .def("AddState", &StateTransitionModel::AddState)
        .def("SubtractState", &StateTransitionModel::SubtractState)
        .def("MultiplyState", &StateTransitionModel::MultiplyState)
        .def("ScalarMultiplyState", &StateTransitionModel::ScalarMultiplyState)
        .def("DivideState", &StateTransitionModel::DivideState)
        .def_static("Create", &StateTransitionModel::Create);
}