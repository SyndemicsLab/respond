#include "gtest/gtest.h"
#include "../src/Simulation.hpp"
#include "mocks/MockSimulation.hpp"

using ::testing::AtLeast;

TEST(SimulationCreation, DefaultConstructor){
    Simulation sim;
    int test;
    EXPECT_NE(sim, &test);
}

TEST(SimulationCreation, MainConstructor){
    Simulation sim(10, 10, 10, 10);
    int test;
    EXPECT_NE(sim, &test);
}

TEST(Loading, EnteringSamples){
    Simulation sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    std::vector<Eigen::Tensor<float, 3>> samples;
    samples.push_back(t1);
    samples.push_back(t2);

}