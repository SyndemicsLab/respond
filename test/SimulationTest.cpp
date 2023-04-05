#include <gtest/gtest.h>
#include "../src/Model/Simulation.hpp"
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

TEST(Loading, InitialGroup){
    Sim sim;
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    t1.setRandom();
    sim.LoadInitialGroup(t1);
    sim.Run();
    Eigen::Tensor<float, 3> result = sim.getHistory().at(0);
    Eigen::Tensor<bool, 0> eq = (t1 == result).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, EnteringSamples){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    DataMatrix samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadEnteringSamples(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetEnteringSamples().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, OUDTransitions){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    DataMatrix samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadOUDTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetOUDTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, TreatmentTransitions){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    DataMatrix samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadTreatmentTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetTreatmentTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, OverdoseTransitions){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    DataMatrix samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadOverdoseTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetOverdoseTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, MortalityTransitions){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    DataMatrix samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadMortalityTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetMortalityTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, TransitionModules){
    Sim sim(10, 10, 10, 10);
    Eigen::Tensor<float, 3> e1(2, 2, 2);
    Eigen::Tensor<float, 3> e2(2, 2, 2);
    e1.setRandom();
    e2.setRandom();
    DataMatrix eVec;
    eVec.push_back(e1);
    eVec.push_back(e2);

    Eigen::Tensor<float, 3> o1(2, 2, 2);
    Eigen::Tensor<float, 3> o2(2, 2, 2);
    o1.setRandom();
    o2.setRandom();
    DataMatrix oVec;
    oVec.push_back(o1);
    oVec.push_back(o2);

    Eigen::Tensor<float, 3> t1(2, 2, 2);
    Eigen::Tensor<float, 3> t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    DataMatrix tVec;
    tVec.push_back(t1);
    tVec.push_back(t2);

    Eigen::Tensor<float, 3> d1(2, 2, 2);
    Eigen::Tensor<float, 3> d2(2, 2, 2);
    d1.setRandom();
    d2.setRandom();
    DataMatrix dVec;
    dVec.push_back(d1);
    dVec.push_back(d2);

    Eigen::Tensor<float, 3> m1(2, 2, 2);
    Eigen::Tensor<float, 3> m2(2, 2, 2);
    m1.setRandom();
    m2.setRandom();
    DataMatrix mVec;
    mVec.push_back(m1);
    mVec.push_back(m2);

    sim.LoadTransitionModules(eVec, oVec, tVec, dVec, mVec);
    
    Eigen::Tensor<bool, 0> eq1 = (eVec.at(0) == sim.GetEnteringSamples().at(0)).all();
    Eigen::Tensor<bool, 0> eq2 = (oVec.at(0) == sim.GetOUDTransitions().at(0)).all();
    Eigen::Tensor<bool, 0> eq3 = (tVec.at(0) == sim.GetTreatmentTransitions().at(0)).all();
    Eigen::Tensor<bool, 0> eq4 = (dVec.at(0) == sim.GetOverdoseTransitions().at(0)).all();
    Eigen::Tensor<bool, 0> eq5 = (mVec.at(0) == sim.GetMortalityTransitions().at(0)).all();
    
    int res = eq1(0) * eq2(0) * eq3(0) * eq4(0) * eq5(0);
    EXPECT_TRUE(res);
}

TEST(Run, EmptyRun){
    Sim sim(0, 0, 0, 0);
    Eigen::Tensor<float, 3> expected(0, 0, 0);
    expected.setZero();
    sim.Run();
    DataMatrix history = sim.getHistory();
    Eigen::Tensor<float, 3> actual = history.at(0);
    Eigen::Tensor<bool, 0> eq = (expected == actual).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Run, SingleStepRun){
    Sim sim(1, 2, 2, 2);

    Eigen::Tensor<float, 3> e1(2, 2, 2);
    e1.setValues( {{{1, 1}, {1, 1}},
                   {{1, 1}, {1, 1}}} );
    DataMatrix eVec;
    eVec.push_back(e1);
    Eigen::Tensor<float, 3> o1(2, 4, 2);
    o1.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );
    DataMatrix oVec;
    oVec.push_back(o1);
    Eigen::Tensor<float, 3> t1(4, 2, 2);
    t1.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    DataMatrix tVec;
    tVec.push_back(t1);
    Eigen::Tensor<float, 3> d1(2, 2, 2);
    d1.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    DataMatrix dVec;
    dVec.push_back(d1);
    Eigen::Tensor<float, 3> m1(2, 2, 2);
    m1.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );
    DataMatrix mVec;
    mVec.push_back(m1);
    sim.LoadTransitionModules(eVec, oVec, tVec, dVec, mVec);

    Eigen::Tensor<float, 3> initialGroup(2, 2, 2);
    initialGroup.setValues( {{{1, 1}, {1, 1}}, 
                             {{1, 1}, {1, 1}}} );
    sim.LoadInitialGroup(initialGroup);

    Eigen::Tensor<float, 3> expected0(2, 2, 2);
    expected0.setValues( {{{1, 1}, {1, 1}},
                          {{1, 1}, {1, 1}}} );

    Eigen::Tensor<float, 3> expected1(2, 2, 2);
    expected1.setValues( {{{1.59885, 2.5047}, {2.18295, 1.6929}},
                          {{1.59885, 2.5047}, {2.18295, 1.6929}}} );
    
    sim.Run();
    DataMatrix history = sim.getHistory();
    Eigen::Tensor<float, 3> actual0 = history.at(0);
    Eigen::Tensor<float, 3> actual1 = history.at(1);
    Eigen::Tensor<bool, 0> eq0 = (expected0 == actual0).all();
    
    Eigen::Tensor<float, 3> res1 = (expected1 - actual1).abs();
    Eigen::Tensor<float, 0> res2 = res1.maximum();
    bool eq1 = (res2(0) <= 0.00001f);

    int res = eq0(0) * eq1;
    EXPECT_TRUE(res);
}
