#include <gtest/gtest.h>
#include "Simulation.hpp"
#include "DataTypes.hpp"
#include "mocks/MockSimulation.hpp"

using namespace Simulation;
using namespace Data;

TEST(SimulationCreation, DefaultConstructor){
    Sim sim;
    int expected = 0;
    EXPECT_EQ(sim.Duration, expected);
}

TEST(SimulationCreation, MainConstructor){
    Sim sim(10, 10, 10, 10);
    int expected = 10;
    EXPECT_EQ(sim.Duration, expected);
}

TEST(Loading, InitialGroup){
    Sim sim;
    Matrix3d t1(2, 2, 2);
    t1.setRandom();
    sim.loadInitialSample(t1);
    sim.Run();
    Matrix3d result = sim.getHistory().stateHistory.getMatrix3dAtTimestep(0);
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
    std::vector<Matrix3d> agg{t1, t2};
    Matrix3dOverTime samples(agg);
    sim.loadEnteringSamples(samples);
    Eigen::Tensor<bool, 0> eq = (samples.getMatrix3dAtTimestep(0) == sim.GetEnteringSamples().getMatrix3dAtTimestep(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, OUDTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    t1.setRandom();
    sim.loadOUDTransitionRates(t1);
    Eigen::Tensor<bool, 0> eq = (t1 == sim.GetOUDTransitions()).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, InterventionTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    std::vector<Matrix3d> agg{t1, t2};
    Matrix3dOverTime samples(agg);
    sim.loadInterventionTransitionRates(samples);
    Eigen::Tensor<bool, 0> eq = (samples.getMatrix3dAtTimestep(0) == sim.GetInterventionTransitions().getMatrix3dAtTimestep(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, OverdoseTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    std::vector<Matrix3d> agg{t1, t2};
    Matrix3dOverTime samples(agg);
    sim.loadOverdoseRates(samples);
    Eigen::Tensor<bool, 0> eq = (samples.getMatrix3dAtTimestep(0) == sim.GetOverdoseTransitions().getMatrix3dAtTimestep(0)).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, MortalityTransitions){
    Sim sim(10, 10, 10, 10);
    Matrix3d t1(2, 2, 2);
    t1.setRandom();
    sim.loadMortalityRates(t1);
    Eigen::Tensor<bool, 0> eq = (t1 == sim.GetMortalityTransitions()).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Loading, TransitionModules){
    Sim sim(10, 10, 10, 10);
    Matrix3d e1(2, 2, 2);
    Matrix3d e2(2, 2, 2);
    e1.setRandom();
    e2.setRandom();
    std::vector<Matrix3d> es{e1, e2};
    Matrix3dOverTime eVec(es);

    Matrix3d o1(2, 2, 2);
    o1.setRandom();

    Matrix3d i1(2, 2, 2);
    i1.setRandom();

    Matrix3d t1(2, 2, 2);
    Matrix3d t2(2, 2, 2);
    t1.setRandom();
    t2.setRandom();
    std::vector<Matrix3d>ts{t1, t2};
    Matrix3dOverTime tVec(ts);

    Matrix3d d1(2, 2, 2);
    Matrix3d d2(2, 2, 2);
    d1.setRandom();
    d2.setRandom();
    std::vector<Matrix3d> ds{d1, d2};
    Matrix3dOverTime dVec(ds);

    Matrix3d fod1(2, 2, 2);
    Matrix3d fod2(2, 2, 2);
    fod1.setRandom();
    fod2.setRandom();
    std::vector<Matrix3d> fods{fod1, fod2};
    Matrix3dOverTime fodVec(fods);

    Matrix3d m1(2, 2, 2);
    m1.setRandom();

    sim.LoadTransitionModules(eVec, o1, i1, tVec, fodVec, dVec, m1);
    
    Eigen::Tensor<bool, 0> eq1 = (eVec.getMatrix3dAtTimestep(0) == sim.GetEnteringSamples().getMatrix3dAtTimestep(0)).all();
    Eigen::Tensor<bool, 0> eq2 = (o1 == sim.GetOUDTransitions()).all();
    Eigen::Tensor<bool, 0> eq3 = (tVec.getMatrix3dAtTimestep(0) == sim.GetInterventionTransitions().getMatrix3dAtTimestep(0)).all();
    Eigen::Tensor<bool, 0> eq4 = (dVec.getMatrix3dAtTimestep(0) == sim.GetOverdoseTransitions().getMatrix3dAtTimestep(0)).all();
    Eigen::Tensor<bool, 0> eq5 = (m1 == sim.GetMortalityTransitions()).all();
    
    int res = eq1(0) * eq2(0) * eq3(0) * eq4(0) * eq5(0);
    EXPECT_TRUE(res);
}

TEST(Run, EmptyRun){
    Sim sim(0, 0, 0, 0);
    Matrix3d expected(0, 0, 0);
    expected.setZero();
    sim.loadInitialSample(expected);
    sim.Run();
    Matrix3d actual = sim.getHistory().stateHistory.getMatrix3dAtTimestep(0);
    Eigen::Tensor<bool, 0> eq = (expected == actual).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Run, SingleStepRun){
    Sim sim(1, 2, 2, 2);

    Matrix3d e1(2, 2, 2);
    e1.setValues( {{{1, 1}, {1, 1}},
                   {{1, 1}, {1, 1}}} );
    std::vector<Matrix3d> es{e1};
    Matrix3dOverTime eVec(es);

    Matrix3d o1(2, 4, 2);
    o1.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );

    Matrix3d i1(2, 4, 2);
    i1.setValues( {{{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}},
                   {{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}}} );

    Matrix3d t1(4, 2, 2);
    t1.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    std::vector<Matrix3d> ts{t1};
    Matrix3dOverTime tVec(ts);

    Matrix3d fod1(2, 2, 2);
    fod1.setValues({{{.0, .0}, {.0, .0}},
                    {{.0, .0}, {.0, .0}}});
    std::vector<Matrix3d> fods{fod1};
    Matrix3dOverTime fodVec(fods);

    Matrix3d d1(2, 2, 2);
    d1.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    std::vector<Matrix3d> ds{d1};
    Matrix3dOverTime dVec(ds);

    Matrix3d m1(2, 2, 2);
    m1.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );
    sim.LoadTransitionModules(eVec, o1, i1, tVec, fodVec, dVec, m1);

    Matrix3d initialSample(2, 2, 2);
    initialSample.setValues( {{{1, 1}, {1, 1}}, 
                             {{1, 1}, {1, 1}}} );
    sim.loadInitialSample(initialSample);

    Matrix3d expected0(2, 2, 2);
    expected0.setValues( {{{1, 1}, {1, 1}},
                          {{1, 1}, {1, 1}}} );

    Matrix3d expected1(2, 2, 2);
    expected1.setValues( {{{1.84077, 2.16312}, {1.94103, 2.03448}},
                          {{2.35114, 2.2921}, {1.43066, 1.9055}}} );
    
    sim.Run();
    History history = sim.getHistory();
    Matrix3d actual0 = history.stateHistory.getMatrix3dAtTimestep(0);
    Matrix3d actual1 = history.stateHistory.getMatrix3dAtTimestep(1);

    Eigen::Tensor<bool, 0> eq0 = (expected0 == actual0).all();
    
    Matrix3d res1 = (expected1 - actual1).abs();
    Eigen::Tensor<double, 0> res2 = res1.maximum();
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
    std::vector<Matrix3d> es{e1, e2, e3};
    Matrix3dOverTime eVec(es);
    
    Matrix3d o1(2, 4, 2);
    o1.setValues( {{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                   {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}} );

    Matrix3d i1(2, 4, 2);
    i1.setValues( {{{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}},
                   {{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}}} );

    Matrix3d t1(4, 2, 2);
    t1.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    Matrix3d t2(4, 2, 2);
    t2.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    Matrix3d t3(4, 2, 2);
    t3.setValues( {{{.8, .2}, {.9, .1}}, {{.8, .2}, {.9, .1}},
                   {{.05, .95}, {.15, .85}}, {{.05, .95}, {.15, .85}}} );
    std::vector<Matrix3d> ts{t1, t2, t3};
    Matrix3dOverTime tVec(ts);

    Matrix3d fod1(2, 2, 2);
    fod1.setValues( {{{.0, .0}, {.0, .0}},
                     {{.0, .0}, {.0, .0}}} );
    Matrix3d fod2(2, 2, 2);
    fod2.setValues( {{{.0, .0}, {.0, .0}},
                     {{.0, .0}, {.0, .0}}} );
    Matrix3d fod3(2, 2, 2);
    fod3.setValues( {{{.0, .0}, {.0, .0}},
                     {{.0, .0}, {.0, .0}}} );
    std::vector<Matrix3d> fods{fod1, fod2, fod3};
    Matrix3dOverTime fodVec(fods);

    Matrix3d d1(2, 2, 2);
    d1.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    Matrix3d d2(2, 2, 2);
    d2.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    Matrix3d d3(2, 2, 2);
    d3.setValues( {{{.25, .25}, {.25, .25}},
                   {{.25, .25}, {.25, .25}}} );
    std::vector<Matrix3d> ds{d1, d2, d3};
    Matrix3dOverTime dVec(ds);

    Matrix3d m1(2, 2, 2);
    m1.setValues( {{{.01, .01}, {.01, .01}},
                   {{.01, .01}, {.01, .01}}} );

    sim.LoadTransitionModules(eVec, o1, i1, tVec, fodVec, dVec, m1);

    Matrix3d initialSample(2, 2, 2);
    initialSample.setValues( {{{1, 1}, {1, 1}}, 
                             {{1, 1}, {1, 1}}} );
    sim.loadInitialSample(initialSample);

    Matrix3d expected0(2, 2, 2);
    expected0.setValues( {{{1, 1}, {1, 1}},
                          {{1, 1}, {1, 1}}} );

    Matrix3d expected1(2, 2, 2);
    expected1.setValues( {{{1.84077, 2.16312}, {1.94103, 2.03448}},
                          {{2.35114, 2.2921}, {1.43066, 1.9055}}} );

    Matrix3d expected2(2, 2, 2);
    expected2.setValues( {{{2.5841, 3.25968}, {2.85186, 3.26194}},
                          {{3.4414, 3.67511}, {1.99455, 2.84651}}} );
    
    Matrix3d expected3(2, 2, 2);
    expected3.setValues( {{{3.26877, 4.39218}, {3.71778, 4.57904}},
                          {{4.46667, 5.15593}, {2.51988, 3.81529}}} );
    
    sim.Run();
    History history = sim.getHistory();
    Matrix3d actual0 = history.stateHistory.getMatrix3dAtTimestep(0);
    Matrix3d actual1 = history.stateHistory.getMatrix3dAtTimestep(1);
    Matrix3d actual2 = history.stateHistory.getMatrix3dAtTimestep(2);
    Matrix3d actual3 = history.stateHistory.getMatrix3dAtTimestep(3);

    Eigen::Tensor<bool, 0> eq0 = (expected0 == actual0).all();

    Matrix3d res1 = (expected1 - actual1).abs();
    Eigen::Tensor<double, 0> res2 = res1.maximum();
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
