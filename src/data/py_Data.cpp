#include "RespondDataStoreImpl.hpp"
#include "data/RespondDataStore.hpp"

#include <Eigen/Eigen>
#include <memory>
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace synmodels::data;

void init_data(py::module &m) {
    py::class_<RespondDataStore, RespondDataStoreImpl>(m, "RespondDataStore")
        .def(py::init<>())
        .def("GetDuration", &RespondDataStore::GetDuration)
        .def("GetStoreHistoryStatus", &RespondDataStore::GetStoreHistoryStatus)
        .def("GetHistoryTimestepsToStore",
             &RespondDataStore::GetHistoryTimestepsToStore)
        .def("SetDuration", &RespondDataStore::SetDuration)
        .def("SetStoreHistoryStatus", &RespondDataStore::SetStoreHistoryStatus)
        .def("SetHistoryTimestepsToStore",
             &RespondDataStore::SetHistoryTimestepsToStore)
        .def("GetInitialCohortState", &RespondDataStore::GetInitialCohortState)
        .def("GetMigratingCohortState",
             &RespondDataStore::GetMigratingCohortState)
        .def("GetBehaviorTransitions",
             &RespondDataStore::GetBehaviorTransitions)
        .def("GetInterventionTransitions",
             &RespondDataStore::GetInterventionTransitions)
        .def("GetBehaviorAfterInterventionTransitions",
             &RespondDataStore::GetBehaviorAfterInterventionTransitions)
        .def("GetOverdoseProbabilityState",
             &RespondDataStore::GetOverdoseProbabilityState)
        .def("GetOverdoseBeingFatalProbabilityState",
             &RespondDataStore::GetOverdoseBeingFatalProbabilityState)
        .def("GetStandardMortalityRatioState",
             &RespondDataStore::GetStandardMortalityRatioState)
        .def("GetBackgroundMortalityState",
             &RespondDataStore::GetBackgroundMortalityState)
        .def("SetInitialCohortState", &RespondDataStore::SetInitialCohortState)
        .def("SetMigratingCohortState",
             &RespondDataStore::SetMigratingCohortState)
        .def("SetBehaviorTransitions",
             &RespondDataStore::SetBehaviorTransitions)
        .def("SetInterventionTransitions",
             &RespondDataStore::SetInterventionTransitions)
        .def("SetBehaviorAfterInterventionTransitions",
             &RespondDataStore::SetBehaviorAfterInterventionTransitions)
        .def("SetOverdoseProbabilityState",
             &RespondDataStore::SetOverdoseProbabilityState)
        .def("SetOverdoseBeingFatalProbabilityState",
             &RespondDataStore::SetOverdoseBeingFatalProbabilityState)
        .def("SetStandardMortalityRatioState",
             &RespondDataStore::SetStandardMortalityRatioState)
        .def("SetBackgroundMortalityState",
             &RespondDataStore::SetBackgroundMortalityState)
        .def_static("Create", &RespondDataStore::Create);
}