#include "gmock/gmock.h"
#include "../src/Simulation.hpp"

class MockSimulation : public ISimulation {
    MOCK_METHOD(void, LoadEnteringSamples, (std::vector<Eigen::Tensor<float, 3>> enteringSamples), (override));
    MOCK_METHOD(void, LoadOUDTransitions, (std::vector<Eigen::Tensor<float, 3>> oudTransitions), (override));
    MOCK_METHOD(void, LoadTreatmentTransitions, (std::vector<Eigen::Tensor<float, 3>> treatmentTransitions), (override));
    MOCK_METHOD(void, LoadOverdoseTransitions, (std::vector<Eigen::Tensor<float, 3>> overdoseTransitions), (override));
    MOCK_METHOD(void, LoadMortalityTransitions, (std::vector<Eigen::Tensor<float, 3>> mortalityTransitions), (override));
    MOCK_METHOD(void, LoadTransitionModules, (std::vector<Eigen::Tensor<float, 3>> enteringSamples, 
        std::vector<Eigen::Tensor<float, 3>> oudTransitions, 
        std::vector<Eigen::Tensor<float, 3>> treatmentTransitions,
        std::vector<Eigen::Tensor<float, 3>> overdoseTransitions,
        std::vector<Eigen::Tensor<float, 3>> mortalityTransitions
    ), (override));
    MOCK_METHOD(void, Run, (), (override));
    MOCK_METHOD(std::vector<Eigen::Tensor<float, 3>>, getHistory, (), (override));
};
