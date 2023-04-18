#include "gmock/gmock.h"
#include "Simulation.hpp"
#include "DataTypes.hpp"

using namespace Simulation;
using namespace Data;

typedef Eigen::Tensor<float, 3> Tensor3D;

class MockSimulation : public ISim {
    MOCK_METHOD(void, LoadInitialGroup, (Tensor3D initialGroup), (override));
    MOCK_METHOD(void, LoadEnteringSamples, (Matrix3dOverTime enteringSamples), (override));
    MOCK_METHOD(void, LoadOUDTransitions, (Matrix3d oudTransitions), (override));
    MOCK_METHOD(void, LoadInterventionTransitions, (Matrix3dOverTime InterventionTransitions), (override));
    MOCK_METHOD(void, LoadOverdoseTransitions, (Matrix3dOverTime overdoseTransitions), (override));
    MOCK_METHOD(void, LoadMortalityTransitions, (Matrix3d mortalityTransitions), (override));

    MOCK_METHOD(Matrix3dOverTime, GetEnteringSamples, (), (override));
    MOCK_METHOD(Matrix3d, GetOUDTransitions, (), (override));
    MOCK_METHOD(Matrix3dOverTime, GetInterventionTransitions, (), (override));
    MOCK_METHOD(Matrix3dOverTime, GetOverdoseTransitions, (), (override));
    MOCK_METHOD(Matrix3d, GetMortalityTransitions, (), (override));

    MOCK_METHOD(void, LoadTransitionModules, (Matrix3dOverTime enteringSamples, 
        Matrix3d oudTransitions, 
        Matrix3dOverTime interventionTransitions,
        Matrix3dOverTime fatalOverdoseTransitions,
        Matrix3dOverTime overdoseTransitions,
        Matrix3d mortalityTransitions
    ), (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(History, getHistory, (), (override));
};
