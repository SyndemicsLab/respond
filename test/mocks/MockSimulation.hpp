#include "gmock/gmock.h"
#include "../src/Simulation.hpp"

using namespace Simulation;

class MockSimulation : public ISim {
    MOCK_METHOD(void, LoadEnteringSamples, (DataMatrix enteringSamples), (override));
    MOCK_METHOD(void, LoadOUDTransitions, (DataMatrix oudTransitions), (override));
    MOCK_METHOD(void, LoadTreatmentTransitions, (DataMatrix treatmentTransitions), (override));
    MOCK_METHOD(void, LoadOverdoseTransitions, (DataMatrix overdoseTransitions), (override));
    MOCK_METHOD(void, LoadMortalityTransitions, (DataMatrix mortalityTransitions), (override));
    MOCK_METHOD(void, LoadTransitionModules, (DataMatrix enteringSamples, 
        DataMatrix oudTransitions, 
        DataMatrix treatmentTransitions,
        DataMatrix overdoseTransitions,
        DataMatrix mortalityTransitions
    ), (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(DataMatrix, getHistory, (), (override));
};
