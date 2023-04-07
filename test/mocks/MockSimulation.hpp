#include "gmock/gmock.h"
#include "../src/Model/Simulation.hpp"

using namespace Simulation;

typedef Eigen::Tensor<float, 3> Tensor3D;

class MockSimulation : public ISim {
    MOCK_METHOD(void, LoadInitialGroup, (Tensor3D initialGroup), (override));
    MOCK_METHOD(void, LoadEnteringSamples, (DataMatrix enteringSamples), (override));
    MOCK_METHOD(void, LoadOUDTransitions, (DataMatrix oudTransitions), (override));
    MOCK_METHOD(void, LoadTreatmentTransitions, (DataMatrix treatmentTransitions), (override));
    MOCK_METHOD(void, LoadOverdoseTransitions, (DataMatrix overdoseTransitions), (override));
    MOCK_METHOD(void, LoadMortalityTransitions, (DataMatrix mortalityTransitions), (override));

    MOCK_METHOD(DataMatrix, GetEnteringSamples, (), (override));
    MOCK_METHOD(DataMatrix, GetOUDTransitions, (), (override));
    MOCK_METHOD(DataMatrix, GetTreatmentTransitions, (), (override));
    MOCK_METHOD(DataMatrix, GetOverdoseTransitions, (), (override));
    MOCK_METHOD(DataMatrix, GetMortalityTransitions, (), (override));

    MOCK_METHOD(void, LoadTransitionModules, (DataMatrix enteringSamples, 
        DataMatrix oudTransitions, 
        DataMatrix treatmentTransitions,
        DataMatrix overdoseTransitions,
        DataMatrix mortalityTransitions
    ), (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(History, getHistory, (), (override));
};
