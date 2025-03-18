////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/model/simulation.hpp>

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "data_loader_mock.hpp"
#include <respond/data_ops/matrices.hpp>

using namespace respond::model;
using namespace respond::data_ops;

using ::testing::NiceMock;
using ::testing::Return;

class RespondTest : public ::testing::Test {
protected:
    NiceMock<MockDataLoader> data_loader;

    std::unique_ptr<Respond> respond;
    Data::IConfigablePtr config;
    std::ofstream config_file_stream;
    void SetUp() override {
        respond = Respond::Create();
        config_file_stream.open("sim.conf");
    }
    void TearDown() override { std::remove("sim.conf"); }
};

TEST_F(RespondTest, FactoryCreate) {
    auto r = Respond::Create();
    EXPECT_NE(nullptr, r);
}

TEST_F(RespondTest, ZeroDuration) {
    config_file_stream << "[simulation]" << std::endl << "duration = 0";
    config = std::make_shared<Data::Config>("sim.conf");

    Matrix3d temp(1, 1, 1);
    temp.setConstant(1.0);
    EXPECT_CALL(data_loader, GetInitialSample()).WillOnce(Return(temp));
    EXPECT_CALL(data_loader, GetConfig()).WillOnce(Return(config));

    respond->Run(data_loader);
}

TEST_F(RespondTest, OneDuration) {
    config_file_stream << "[simulation]" << std::endl
                       << "duration = 1" << std::endl
                       << "[state]" << std::endl
                       << "interventions = No_Treatment" << std::endl
                       << "ouds = Active_Injection";

    Matrix3d temp(1, 1, 1);
    temp.setConstant(1.0);
    TimedMatrix3d temp_timed = {{0, temp}};

    EXPECT_CALL(data_loader, GetInitialSample()).WillOnce(Return(temp));
    EXPECT_CALL(data_loader, GetConfig())
        .Times(3)
        .WillRepeatedly(Return(config));

    EXPECT_CALL(data_loader, GetEnteringSamples()).WillOnce(Return(temp_timed));

    temp.setConstant(0.5);
    EXPECT_CALL(data_loader, GetOUDTransitionRates()).WillOnce(Return(temp));

    temp_timed = {{0, temp}};
    EXPECT_CALL(data_loader, GetInterventionTransitionRates())
        .WillOnce(Return(temp_timed));

    EXPECT_CALL(data_loader, GetInterventionInitRates()).WillOnce(Return(temp));

    EXPECT_CALL(data_loader, GetOverdoseRates()).WillOnce(Return(temp_timed));

    EXPECT_CALL(data_loader, GetFatalOverdoseRates())
        .WillOnce(Return(temp_timed));

    EXPECT_CALL(data_loader, GetMortalityRates()).WillOnce(Return(temp));

    respond->Run(data_loader);
    auto history = respond->GetHistory();

    EXPECT_TRUE(history.state_history.empty());
}

TEST_F(RespondTest, TwoDuration) {
    config_file_stream << "[simulation]" << std::endl
                       << "duration = 1" << std::endl
                       << "[state]" << std::endl
                       << "interventions = No_Treatment" << std::endl
                       << "ouds = Active_Injection";

    Matrix3d temp(1, 1, 1);
    temp.setConstant(1.0);
    TimedMatrix3d temp_timed = {{0, temp}};

    EXPECT_CALL(data_loader, GetInitialSample()).WillOnce(Return(temp));
    EXPECT_CALL(data_loader, GetConfig())
        .Times(5)
        .WillRepeatedly(Return(config));

    EXPECT_CALL(data_loader, GetEnteringSamples())
        .Times(2)
        .WillRepeatedly(Return(temp_timed));

    temp.setConstant(0.5);
    EXPECT_CALL(data_loader, GetOUDTransitionRates())
        .Times(2)
        .WillRepeatedly(Return(temp));

    temp_timed = {{0, temp}};
    EXPECT_CALL(data_loader, GetInterventionTransitionRates())
        .Times(2)
        .WillRepeatedly(Return(temp_timed));

    EXPECT_CALL(data_loader, GetInterventionInitRates())
        .Times(2)
        .WillRepeatedly(Return(temp));

    EXPECT_CALL(data_loader, GetOverdoseRates())
        .Times(2)
        .WillRepeatedly(Return(temp_timed));

    EXPECT_CALL(data_loader, GetFatalOverdoseRates())
        .Times(2)
        .WillRepeatedly(Return(temp_timed));

    EXPECT_CALL(data_loader, GetMortalityRates())
        .Times(2)
        .WillRepeatedly(Return(temp));

    respond->Run(data_loader);
    auto history = respond->GetHistory();

    EXPECT_TRUE(history.state_history.empty());
}
