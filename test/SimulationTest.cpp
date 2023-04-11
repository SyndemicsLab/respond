#include <gtest/gtest.h>
#include "Simulation.hpp"
#include "DataTypes.hpp"
#include "mocks/MockSimulation.hpp"

using namespace Simulation;
using namespace Data;

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
    Matrix3d t1(2, 2, 2);
    t1.setRandom();
    sim.LoadInitialGroup(t1);
    sim.Run();
    Matrix3d result = sim.getHistory().stateHistory.at(0);
    Eigen::Tensor<bool, 0> eq = (t1 == result).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, EnteringSamples){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    Matrix3dOverTime samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadEnteringSamples(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetEnteringSamples().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, OUDTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    Matrix3dOverTime samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadOUDTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetOUDTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, TreatmentTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    Matrix3dOverTime samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadTreatmentTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetTreatmentTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, OverdoseTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    Matrix3dOverTime samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadOverdoseTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetOverdoseTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, MortalityTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    Matrix3dOverTime samples;
    samples.push_back(t1);
    samples.push_back(t2);
    sim.LoadMortalityTransitions(samples);
    Eigen::Tensor<bool, 0> eq = (samples.at(0) == sim.GetMortalityTransitions().at(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, TransitionModules){
    Sim sim(10, 10, 10, 10);
    Matrix3d e1(2, 2, 2);
    Matrix3d e2(2, 2, 2);
    e1.setRandom();
    e2.setRandom();
    Matrix3dOverTime eVec;
    eVec.push_back(e1);
    eVec.push_back(e2);

    Matrix3d o1(2, 2, 2);
    Matrix3d o2(2, 2, 2);
    o1.setRandom();
    o2.setRandom();
    Matrix3dOverTime oVec;
    oVec.push_back(o1);
    oVec.push_back(o2);

    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    Matrix3dOverTime tVec;
    tVec.push_back(t1);
    tVec.push_back(t2);

    Matrix3d d1(2, 2, 2);
    Matrix3d d2(2, 2, 2);
    d1.setRandom();
    d2.setRandom();
    Matrix3dOverTime dVec;
    dVec.push_back(d1);
    dVec.push_back(d2);

    Matrix3d m1(2, 2, 2);
    Matrix3d m2(2, 2, 2);
    m1.setRandom();
    m2.setRandom();
    Matrix3dOverTime mVec;
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
    Matrix3d expected(0, 0, 0);
    expected.setZero();
    sim.LoadInitialGroup(expected);
    sim.Run();
    Matrix3d actual = sim.getHistory().stateHistory.at(0);
    Eigen::Tensor<bool, 0> eq = (expected == actual).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Run, SingleStepRun){
    Sim sim(1, 2, 2, 2);

    Matrix3d e1(2, 2, 2);
    e1.setValues( {{{1, 1}, {1, 1}},
                   {{1, 1}, {1, 1}}} );
    Matrix3dOverTime eVec;
    eVec.push_back(e1);
    Matrix3d o1(2, 4, 2);
    o1.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );
    Matrix3dOverTime oVec;
    oVec.push_back(o1);
    Matrix3d t1(4, 2, 2);
    t1.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    Matrix3dOverTime tVec;
    tVec.push_back(t1);
    Matrix3d d1(2, 2, 2);
    d1.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    Matrix3dOverTime dVec;
    dVec.push_back(d1);
    Matrix3d m1(2, 2, 2);
    m1.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );
    Matrix3dOverTime mVec;
    mVec.push_back(m1);
    sim.LoadTransitionModules(eVec, oVec, tVec, dVec, mVec);

    Matrix3d initialGroup(2, 2, 2);
    initialGroup.setValues( {{{1, 1}, {1, 1}}, 
                             {{1, 1}, {1, 1}}} );
    sim.LoadInitialGroup(initialGroup);

    Matrix3d expected0(2, 2, 2);
    expected0.setValues( {{{1, 1}, {1, 1}},
                          {{1, 1}, {1, 1}}} );

    Matrix3d expected1(2, 2, 2);
    expected1.setValues( {{{1.59885, 2.5047}, {2.18295, 1.6929}},
                          {{1.59885, 2.5047}, {2.18295, 1.6929}}} );
    
    sim.Run();
    History history = sim.getHistory();
    Matrix3d actual0 = history.stateHistory.at(0);
    Matrix3d actual1 = history.stateHistory.at(1);
    Eigen::Tensor<bool, 0> eq0 = (expected0 == actual0).all();
    
    Matrix3d res1 = (expected1 - actual1).abs();
    Eigen::Tensor<float, 0> res2 = res1.maximum();
    bool eq1 = (res2(0) <= 0.00001f);

    int res = eq0(0) * eq1;
    EXPECT_TRUE(res);
}

TEST(Run, MultiStepRun){
    Sim sim(3, 2, 2, 2);

    Matrix3d e1(2, 2, 2);
    e1.setValues( {{{1, 1}, {1, 1}},
                   {{1, 1}, {1, 1}}} );
    Matrix3d e2(2, 2, 2);
    e2.setValues( {{{1, 1}, {1, 1}},
                   {{1, 1}, {1, 1}}} );
    Matrix3d e3(2, 2, 2);
    e3.setValues( {{{1, 1}, {1, 1}},
                   {{1, 1}, {1, 1}}} );
    Matrix3dOverTime eVec;
    eVec.push_back(e1);
    eVec.push_back(e2);
    eVec.push_back(e3);

    Matrix3d o1(2, 4, 2);
    o1.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );
    Matrix3d o2(2, 4, 2);
    o2.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );
    Matrix3d o3(2, 4, 2);
    o3.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );
    Matrix3dOverTime oVec;
    oVec.push_back(o1);
    oVec.push_back(o2);
    oVec.push_back(o3);

    Matrix3d t1(4, 2, 2);
    t1.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    Matrix3d t2(4, 2, 2);
    t2.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    Matrix3d t3(4, 2, 2);
    t3.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    Matrix3dOverTime tVec;
    tVec.push_back(t1);
    tVec.push_back(t2);
    tVec.push_back(t3);

    Matrix3d d1(2, 2, 2);
    d1.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    Matrix3d d2(2, 2, 2);
    d2.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    Matrix3d d3(2, 2, 2);
    d3.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    Matrix3dOverTime dVec;
    dVec.push_back(d1);
    dVec.push_back(d2);
    dVec.push_back(d3);

    Matrix3d m1(2, 2, 2);
    m1.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );
    Matrix3d m2(2, 2, 2);
    m2.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );
    Matrix3d m3(2, 2, 2);
    m3.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );
    Matrix3dOverTime mVec;
    mVec.push_back(m1);
    mVec.push_back(m2);
    mVec.push_back(m3);

    sim.LoadTransitionModules(eVec, oVec, tVec, dVec, mVec);

    Matrix3d initialGroup(2, 2, 2);
    initialGroup.setValues( {{{1, 1}, {1, 1}}, 
                             {{1, 1}, {1, 1}}} );
    sim.LoadInitialGroup(initialGroup);

    Matrix3d expected0(2, 2, 2);
    expected0.setValues( {{{1, 1}, {1, 1}},
                          {{1, 1}, {1, 1}}} );

    Matrix3d expected1(2, 2, 2);
    expected1.setValues( {{{1.59885, 2.5047}, {2.18295, 1.6929}},
                          {{1.59885, 2.5047}, {2.18295, 1.6929}}} );

    Matrix3d expected2(2, 2, 2);
    expected2.setValues( {{{2.12674, 4.20426}, {3.38303, 2.35576}},
                          {{2.12674, 4.20426}, {3.38303, 2.35576}}} );
    
    Matrix3d expected3(2, 2, 2);
    expected3.setValues( {{{2.60531, 6.09665}, {4.58808, 3.01433}},
                          {{2.60531, 6.09665}, {4.58808, 3.01433}}} );
    
    sim.Run();
    History history = sim.getHistory();
    Matrix3d actual0 = history.stateHistory.at(0);
    Matrix3d actual1 = history.stateHistory.at(1);
    Matrix3d actual2 = history.stateHistory.at(2);
    Matrix3d actual3 = history.stateHistory.at(3);

    Eigen::Tensor<bool, 0> eq0 = (expected0 == actual0).all();

    Matrix3d res1 = (expected1 - actual1).abs();
    Eigen::Tensor<float, 0> res2 = res1.maximum();
    bool eq1 = (res2(0) <= 0.00001f);

    res1 = (expected2 - actual2).abs();
    res2 = res1.maximum();
    bool eq2 = (res2(0) <= 0.00001f);

    res1 = (expected3 - actual3).abs();
    res2 = res1.maximum();
    bool eq3 = (res2(0) <= 0.00001f);

    int res = eq0(0) * eq1 * eq2 * eq3;
    EXPECT_TRUE(res);
}
