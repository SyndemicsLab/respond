////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: src                                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-06                                                  //
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

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class RespondTest : public ::testing::Test {
protected:
    NiceMock<MockDataLoader> data_loader;

    std::unique_ptr<Respond> respond;
    Data::IConfigablePtr config;
    std::ofstream config_file_stream;
    void SetUp() override { respond = Respond::Create(); }
    void TearDown() override { std::remove("sim.conf"); }
};

TEST_F(RespondTest, ZeroDuration) {
    config_file_stream.open("sim.conf");
    config_file_stream << "[simulation]" << std::endl
                       << "duration = 0" << std::endl;
    config_file_stream.close();
    config = std::make_shared<Data::Config>("sim.conf");

    Matrix3d temp(1, 1, 1);
    temp.setConstant(1.0);
    EXPECT_CALL(data_loader, GetInitialSample()).WillOnce(Return(temp));
    EXPECT_CALL(data_loader, GetConfig()).WillRepeatedly(Return(config));

    respond->Run(data_loader);
}

TEST_F(RespondTest, OneDuration) {
    config_file_stream.open("sim.conf");
    config_file_stream << "[simulation]" << std::endl
                       << "duration = 1" << std::endl
                       << std::endl
                       << "[state]" << std::endl
                       << "interventions = No_Treatment" << std::endl
                       << "behaviors = Active_Injection";
    config_file_stream.close();
    config = std::make_shared<Data::Config>("sim.conf");

    Matrix3d temp(1, 1, 1);
    temp.setConstant(1.0);

    EXPECT_CALL(data_loader, GetInitialSample()).WillOnce(Return(temp));
    EXPECT_CALL(data_loader, GetConfig())
        .Times(3)
        .WillRepeatedly(Return(config));

    EXPECT_CALL(data_loader, GetEnteringSamples(_)).WillOnce(Return(temp));

    temp.setConstant(0.5);
    EXPECT_CALL(data_loader, GetOUDTransitionRates()).WillOnce(Return(temp));

    EXPECT_CALL(data_loader, GetInterventionTransitionRates(_))
        .WillOnce(Return(temp));

    EXPECT_CALL(data_loader, GetInterventionInitRates())
        .Times(1)
        .WillOnce(Return(temp));

    EXPECT_CALL(data_loader, GetOverdoseRates(_)).WillOnce(Return(temp));

    EXPECT_CALL(data_loader, GetFatalOverdoseRates(_)).WillOnce(Return(temp));

    EXPECT_CALL(data_loader, GetMortalityRates()).WillOnce(Return(temp));

    respond->Run(data_loader);
    auto history = respond->GetHistory();

    EXPECT_FALSE(history.state_history.empty());
}

TEST_F(RespondTest, TwoDuration) {
    config_file_stream.open("sim.conf");
    config_file_stream
        << "[simulation]" << std::endl
        << "duration = 2" << std::endl
        << std::endl
        << "[state]" << std::endl
        << "interventions = No_Treatment, Buprenorphine, Post_Buprenorphine"
        << std::endl
        << "behaviors = Active_Injection, Nonactive_Injection";
    config_file_stream.close();

    config = std::make_shared<Data::Config>("sim.conf");
    EXPECT_CALL(data_loader, GetConfig()).WillRepeatedly(Return(config));

    Matrix3d ic_mat(3, 2, 1);
    ic_mat.setConstant(1.0);
    EXPECT_CALL(data_loader, GetInitialSample()).WillOnce(Return(ic_mat));

    Matrix3d es_mat(3, 2, 1);
    es_mat.setConstant(1.0);
    EXPECT_CALL(data_loader, GetEnteringSamples(_))
        .Times(2)
        .WillRepeatedly(Return(es_mat));

    Matrix3d squared_behavior_mat(3, 4, 1);
    squared_behavior_mat.setConstant(0.5);
    EXPECT_CALL(data_loader, GetOUDTransitionRates())
        .Times(2)
        .WillRepeatedly(Return(squared_behavior_mat));

    Matrix3d squared_intervention_mat(9, 2, 1);
    EXPECT_CALL(data_loader, GetInterventionTransitionRates(_))
        .Times(2)
        .WillRepeatedly(Return(squared_intervention_mat));

    Matrix3d squared_iie_mat(3, 4, 1);
    squared_iie_mat.setConstant(0.5);
    EXPECT_CALL(data_loader, GetInterventionInitRates())
        .Times(6)
        .WillRepeatedly(Return(squared_iie_mat));

    Matrix3d od_mat(3, 2, 1);
    od_mat.setConstant(0.5);
    EXPECT_CALL(data_loader, GetOverdoseRates(_))
        .Times(2)
        .WillRepeatedly(Return(od_mat));

    EXPECT_CALL(data_loader, GetFatalOverdoseRates(_))
        .Times(2)
        .WillRepeatedly(Return(od_mat));

    EXPECT_CALL(data_loader, GetMortalityRates())
        .Times(2)
        .WillRepeatedly(Return(od_mat));

    respond->Run(data_loader);
    auto history = respond->GetHistory();

    EXPECT_FALSE(history.state_history.empty());
}
