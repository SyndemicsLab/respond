////////////////////////////////////////////////////////////////////////////////
// File: TEST_Simulation.cpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "MockCostLoader.hpp"
#include "MockDataLoader.hpp"
#include "MockSimulation.hpp"
#include "MockUtilityLoader.hpp"

#include <respondsimulation/data_ops/DataTypes.hpp>
#include <respondsimulation/model/Simulation.hpp>

#include <gtest/gtest.h>
#include <memory>

using namespace Simulation;
using namespace data_ops;

using ::testing::Return;

TEST(SimulationCreation, DefaultConstructor) {
    Respond sim;
    int expected = 0;
    EXPECT_EQ(sim.getDuration(), expected);
}

TEST(SimulationCreation, MainConstructor) {
    std::shared_ptr<MockDataLoader> mockLoaderPtr =
        std::make_shared<MockDataLoader>();
    EXPECT_CALL(*mockLoaderPtr, getDuration()).WillRepeatedly(Return(10));
    EXPECT_CALL(*mockLoaderPtr, getNumOUDStates()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getNumInterventions())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getNumDemographicCombos())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getLogger()).WillRepeatedly(Return(nullptr));

    Respond sim(mockLoaderPtr);
    int expected = 10;
    EXPECT_EQ(sim.getDuration(), expected);
}

TEST(Run, EmptyRun) {
    std::shared_ptr<MockDataLoader> mockLoaderPtr =
        std::make_shared<MockDataLoader>();
    EXPECT_CALL(*mockLoaderPtr, getDuration()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getNumOUDStates()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getNumInterventions())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getNumDemographicCombos())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getLogger()).WillRepeatedly(Return(nullptr));

    data_ops::Matrix3d expected(1, 1, 1);
    expected.setZero();
    std::vector<data_ops::Matrix3d> vecmat = {expected};
    data_ops::Matrix4d mat4d(vecmat);

    EXPECT_CALL(*mockLoaderPtr, getInitialSample())
        .WillRepeatedly(Return(expected));
    EXPECT_CALL(*mockLoaderPtr, getEnteringSamples())
        .WillRepeatedly(Return(mat4d));
    EXPECT_CALL(*mockLoaderPtr, getOUDTransitionRates())
        .WillRepeatedly(Return(expected));
    EXPECT_CALL(*mockLoaderPtr, getInterventionTransitionRates())
        .WillRepeatedly(Return(mat4d));
    EXPECT_CALL(*mockLoaderPtr, getOverdoseRates())
        .WillRepeatedly(Return(mat4d));
    EXPECT_CALL(*mockLoaderPtr, getFatalOverdoseRates())
        .WillRepeatedly(Return(mat4d));
    EXPECT_CALL(*mockLoaderPtr, getMortalityRates())
        .WillRepeatedly(Return(expected));
    EXPECT_CALL(*mockLoaderPtr, getInterventionInitRates())
        .WillRepeatedly(Return(expected));
    EXPECT_CALL(*mockLoaderPtr, getAgingInterval()).WillRepeatedly(Return(260));

    std::shared_ptr<MockCostLoader> mockCostLoaderPtr =
        std::make_shared<MockCostLoader>();

    std::shared_ptr<MockUtilityLoader> mockUtilLoaderPtr =
        std::make_shared<MockUtilityLoader>();

    Respond sim(mockLoaderPtr);
    sim.setCost(mockCostLoaderPtr);
    sim.setUtility(mockUtilLoaderPtr);
    sim.run();
    Matrix3d actual = sim.getHistory().stateHistory(0);
    Eigen::Tensor<bool, 0> eq = (expected == actual).all();
    int res = eq(0);
    EXPECT_TRUE(res);
}

TEST(Run, SingleStepRun) {
    std::shared_ptr<MockDataLoader> mockLoaderPtr =
        std::make_shared<MockDataLoader>();
    EXPECT_CALL(*mockLoaderPtr, getDuration()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mockLoaderPtr, getNumOUDStates()).WillRepeatedly(Return(2));
    EXPECT_CALL(*mockLoaderPtr, getNumInterventions())
        .WillRepeatedly(Return(2));
    EXPECT_CALL(*mockLoaderPtr, getNumDemographicCombos())
        .WillRepeatedly(Return(2));
    EXPECT_CALL(*mockLoaderPtr, getLogger()).WillRepeatedly(Return(nullptr));

    Matrix3d initialSample(2, 2, 2);
    initialSample.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});
    EXPECT_CALL(*mockLoaderPtr, getInitialSample())
        .WillRepeatedly(Return(initialSample));

    data_ops::Matrix3d e1(2, 2, 2);
    e1.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});
    std::vector<Matrix3d> es{e1};
    Matrix4d eVec(es);
    EXPECT_CALL(*mockLoaderPtr, getEnteringSamples())
        .WillRepeatedly(Return(eVec));

    data_ops::Matrix3d o1(2, 4, 2);
    o1.setValues({{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                  {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}});
    EXPECT_CALL(*mockLoaderPtr, getOUDTransitionRates())
        .WillRepeatedly(Return(o1));

    Matrix3d t1(4, 2, 2);
    t1.setValues({{{.8, .2}, {.9, .1}},
                  {{.8, .2}, {.9, .1}},
                  {{.05, .95}, {.15, .85}},
                  {{.05, .95}, {.15, .85}}});
    std::vector<Matrix3d> ts{t1};
    Matrix4d tVec(ts);
    EXPECT_CALL(*mockLoaderPtr, getInterventionTransitionRates())
        .WillRepeatedly(Return(ts));

    Matrix3d d1(2, 2, 2);
    d1.setValues({{{.25, .25}, {.25, .25}}, {{.25, .25}, {.25, .25}}});
    std::vector<Matrix3d> ds{d1};
    Matrix4d dVec(ds);
    EXPECT_CALL(*mockLoaderPtr, getOverdoseRates())
        .WillRepeatedly(Return(dVec));

    Matrix3d fod1(2, 2, 2);
    fod1.setValues({{{.0, .0}, {.0, .0}}, {{.0, .0}, {.0, .0}}});
    std::vector<Matrix3d> fods{fod1};
    Matrix4d fodVec(fods);
    EXPECT_CALL(*mockLoaderPtr, getFatalOverdoseRates())
        .WillRepeatedly(Return(fods));

    Matrix3d m1(2, 2, 2);
    m1.setValues({{{.01, .01}, {.01, .01}}, {{.01, .01}, {.01, .01}}});
    EXPECT_CALL(*mockLoaderPtr, getMortalityRates()).WillRepeatedly(Return(m1));

    Matrix3d i1(2, 4, 2);
    i1.setValues(
        {{{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}},
         {{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}}});
    EXPECT_CALL(*mockLoaderPtr, getInterventionInitRates())
        .WillRepeatedly(Return(i1));
    EXPECT_CALL(*mockLoaderPtr, getAgingInterval()).WillRepeatedly(Return(260));

    std::shared_ptr<MockCostLoader> mockCostLoaderPtr =
        std::make_shared<MockCostLoader>();

    std::shared_ptr<MockUtilityLoader> mockUtilLoaderPtr =
        std::make_shared<MockUtilityLoader>();

    Respond sim(mockLoaderPtr);
    sim.setCost(mockCostLoaderPtr);
    sim.setUtility(mockUtilLoaderPtr);

    sim.run();

    Matrix3d expected0(2, 2, 2);
    expected0.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});

    Matrix3d expected1(2, 2, 2);
    expected1.setValues({{{1.84077, 2.16312}, {1.94103, 2.03448}},
                         {{2.35114, 2.2921}, {1.43066, 1.9055}}});

    History history = sim.getHistory();
    Matrix3d actual0 = history.stateHistory(0);
    Matrix3d actual1 = history.stateHistory(1);

    Eigen::Tensor<bool, 0> eq0 = (expected0 == actual0).all();

    Matrix3d res1 = (expected1 - actual1).abs();
    Eigen::Tensor<double, 0> res2 = res1.maximum();
    bool eq1 = (res2(0) <= 0.00001f);

    int res = eq0(0) * eq1;
    EXPECT_TRUE(res);
}

TEST(Run, MultiStepRun) {
    std::shared_ptr<MockDataLoader> mockLoaderPtr =
        std::make_shared<MockDataLoader>();
    EXPECT_CALL(*mockLoaderPtr, getDuration()).WillRepeatedly(Return(3));
    EXPECT_CALL(*mockLoaderPtr, getNumOUDStates()).WillRepeatedly(Return(2));
    EXPECT_CALL(*mockLoaderPtr, getNumInterventions())
        .WillRepeatedly(Return(2));
    EXPECT_CALL(*mockLoaderPtr, getNumDemographicCombos())
        .WillRepeatedly(Return(2));
    EXPECT_CALL(*mockLoaderPtr, getLogger()).WillRepeatedly(Return(nullptr));

    Matrix3d initialSample(2, 2, 2);
    initialSample.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});
    EXPECT_CALL(*mockLoaderPtr, getInitialSample())
        .WillRepeatedly(Return(initialSample));

    data_ops::Matrix3d e1(2, 2, 2);
    e1.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});
    Matrix3d e2(2, 2, 2);
    e2.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});
    Matrix3d e3(2, 2, 2);
    e3.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});
    std::vector<Matrix3d> es{e1, e2, e3};
    Matrix4d eVec(es);
    EXPECT_CALL(*mockLoaderPtr, getEnteringSamples())
        .WillRepeatedly(Return(eVec));

    data_ops::Matrix3d o1(2, 4, 2);
    o1.setValues({{{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}},
                  {{.85, .9}, {.15, .1}, {.1, .2}, {.9, .8}}});
    EXPECT_CALL(*mockLoaderPtr, getOUDTransitionRates())
        .WillRepeatedly(Return(o1));

    Matrix3d t1(4, 2, 2);
    t1.setValues({{{.8, .2}, {.9, .1}},
                  {{.8, .2}, {.9, .1}},
                  {{.05, .95}, {.15, .85}},
                  {{.05, .95}, {.15, .85}}});
    Matrix3d t2(4, 2, 2);
    t2.setValues({{{.8, .2}, {.9, .1}},
                  {{.8, .2}, {.9, .1}},
                  {{.05, .95}, {.15, .85}},
                  {{.05, .95}, {.15, .85}}});
    Matrix3d t3(4, 2, 2);
    t3.setValues({{{.8, .2}, {.9, .1}},
                  {{.8, .2}, {.9, .1}},
                  {{.05, .95}, {.15, .85}},
                  {{.05, .95}, {.15, .85}}});
    std::vector<Matrix3d> ts{t1, t2, t3};
    Matrix4d tVec(ts);
    EXPECT_CALL(*mockLoaderPtr, getInterventionTransitionRates())
        .WillRepeatedly(Return(ts));

    Matrix3d d1(2, 2, 2);
    d1.setValues({{{.25, .25}, {.25, .25}}, {{.25, .25}, {.25, .25}}});
    Matrix3d d2(2, 2, 2);
    d2.setValues({{{.25, .25}, {.25, .25}}, {{.25, .25}, {.25, .25}}});
    Matrix3d d3(2, 2, 2);
    d3.setValues({{{.25, .25}, {.25, .25}}, {{.25, .25}, {.25, .25}}});
    std::vector<Matrix3d> ds{d1, d2, d3};
    Matrix4d dVec(ds);
    EXPECT_CALL(*mockLoaderPtr, getOverdoseRates())
        .WillRepeatedly(Return(dVec));

    Matrix3d fod1(2, 2, 2);
    fod1.setValues({{{.0, .0}, {.0, .0}}, {{.0, .0}, {.0, .0}}});
    Matrix3d fod2(2, 2, 2);
    fod2.setValues({{{.0, .0}, {.0, .0}}, {{.0, .0}, {.0, .0}}});
    Matrix3d fod3(2, 2, 2);
    fod3.setValues({{{.0, .0}, {.0, .0}}, {{.0, .0}, {.0, .0}}});
    std::vector<Matrix3d> fods{fod1, fod2, fod3};
    Matrix4d fodVec(fods);
    EXPECT_CALL(*mockLoaderPtr, getFatalOverdoseRates())
        .WillRepeatedly(Return(fods));

    Matrix3d m1(2, 2, 2);
    m1.setValues({{{.01, .01}, {.01, .01}}, {{.01, .01}, {.01, .01}}});
    EXPECT_CALL(*mockLoaderPtr, getMortalityRates()).WillRepeatedly(Return(m1));

    Matrix3d i1(2, 4, 2);
    i1.setValues(
        {{{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}},
         {{0.256506, 0.102857}, {0.743494, 0.897143}, {1.0, 1.0}, {0.0, 0.0}}});
    EXPECT_CALL(*mockLoaderPtr, getInterventionInitRates())
        .WillRepeatedly(Return(i1));
    EXPECT_CALL(*mockLoaderPtr, getAgingInterval()).WillRepeatedly(Return(260));

    std::shared_ptr<MockCostLoader> mockCostLoaderPtr =
        std::make_shared<MockCostLoader>();

    std::shared_ptr<MockUtilityLoader> mockUtilLoaderPtr =
        std::make_shared<MockUtilityLoader>();

    Respond sim(mockLoaderPtr);
    sim.setCost(mockCostLoaderPtr);
    sim.setUtility(mockUtilLoaderPtr);

    sim.run();

    Matrix3d expected0(2, 2, 2);
    expected0.setValues({{{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}});

    Matrix3d expected1(2, 2, 2);
    expected1.setValues({{{1.84077, 2.16312}, {1.94103, 2.03448}},
                         {{2.35114, 2.2921}, {1.43066, 1.9055}}});

    Matrix3d expected2(2, 2, 2);
    expected2.setValues({{{2.5841, 3.25968}, {2.85186, 3.26194}},
                         {{3.4414, 3.67511}, {1.99455, 2.84651}}});

    Matrix3d expected3(2, 2, 2);
    expected3.setValues({{{3.26877, 4.39218}, {3.71778, 4.57904}},
                         {{4.46667, 5.15593}, {2.51988, 3.81529}}});

    History history = sim.getHistory();
    Matrix3d actual0 = history.stateHistory(0);
    Matrix3d actual1 = history.stateHistory(1);
    Matrix3d actual2 = history.stateHistory(2);
    Matrix3d actual3 = history.stateHistory(3);

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
