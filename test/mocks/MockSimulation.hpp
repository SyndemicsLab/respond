#include "gmock/gmock.h"
#include "Simulation.hpp"
#include "DataTypes.hpp"

using namespace Simulation;
using namespace Data;

typedef Eigen::Tensor<float, 3> Tensor3D;

class MockSimulation : public ISim {
    MOCK_METHOD(void, loadInitialSample, (Tensor3D initialSample), (override));
    MOCK_METHOD(void, loadEnteringSamples, (Matrix3dOverTime enteringSamples), (override));
    MOCK_METHOD(void, loadOUDTransitionRates, (Matrix3d oudTransitionRates), (override));
    MOCK_METHOD(void, loadInterventionTransitionRates, (Matrix3dOverTime InterventionTransitions), (override));
    MOCK_METHOD(void, loadOverdoseRates, (Matrix3dOverTime overdoseRates), (override));
    MOCK_METHOD(void, loadMortalityRates, (Matrix3d mortalityRates), (override));

    MOCK_METHOD(Matrix3dOverTime, GetEnteringSamples, (), (override));
    MOCK_METHOD(Matrix3d, GetOUDTransitions, (), (override));
    MOCK_METHOD(Matrix3dOverTime, GetInterventionTransitions, (), (override));
    MOCK_METHOD(Matrix3dOverTime, GetOverdoseTransitions, (), (override));
    MOCK_METHOD(Matrix3d, GetMortalityTransitions, (), (override));

    MOCK_METHOD(void, LoadTransitionModules, (Matrix3dOverTime enteringSamples, 
        Matrix3d oudTransitionRates, 
        Matrix3dOverTime interventionTransitionRates,
        Matrix3dOverTime fatalOverdoseRates,
        Matrix3dOverTime overdoseRates,
        Matrix3d mortalityRates
    ), (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(History, getHistory, (), (override));
};
