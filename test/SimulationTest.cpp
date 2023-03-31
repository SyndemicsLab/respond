#include "gtest/gtest.h"
#include "../src/Simulation.hpp"

TEST(SimulationCreation, DefaultConstructor){
    Simulation sim;
    int test;
    EXPECT_NE(sim, &test);
}

TEST(SimulationCreation, MainConstructor){
    Simulation *sim = new Simulation(10, 10, 10, 10);
    int test;
    EXPECT_NE(sim, &test);
}