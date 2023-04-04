#include <gtest/gtest.h>
#include "../src/Simulation.hpp"
#include "mocks/MockSimulation.hpp"

using namespace Simulation; 

TEST(SimulationCreation, DefaultConstructor){
    Sim sim;
    uint16_t expected = 0;
    EXPECT_EQ(sim.Duration, expected);
}

TEST(SimulationCreation, MainConstructor){
    Sim sim(10, 10, 10, 10);
    uint16_t expected = 10;
    EXPECT_EQ(sim.Duration, expected);
}

TEST(Loading, EnteringSamples){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    std::vector<Eigen::Tensor<float, 3>> samples;
    samples.push_back(t1);
    samples.push_back(t2);

}