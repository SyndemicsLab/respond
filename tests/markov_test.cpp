////////////////////////////////////////////////////////////////////////////////
// File: markov_test.cpp                                                      //
// Project: respond                                                           //
// Created Date: 2025-06-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-01-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <respond/markov.hpp>

#include <fstream>
#include <iostream>
#include <memory>

#include <gtest/gtest.h>

using namespace respond;

class MarkovTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MarkovTest, ZeroDuration) {
    auto markov = Markov::Create("test_logger");
    markov->Run(0);
    auto results = markov->GetRunResults();

    auto expected = Eigen::VectorXd::Zero(0);
    EXPECT_TRUE(results[0].state.isApprox(expected));
    EXPECT_TRUE(results[0].intervention_admissions.isApprox(expected));
    EXPECT_TRUE(results[0].total_overdoses.isApprox(expected));
}

TEST_F(MarkovTest, SingleStep) {
    auto markov = Markov::Create("test_logger");
    markov->SetState(Eigen::VectorXd::Ones(5));

    transition t = {[](const Eigen::VectorXd &state,
                       const std::vector<Eigen::MatrixXd> &transitions,
                       HistoryStamp &hs) { return state + transitions[0]; },
                    {Eigen::VectorXd::Ones(5)}};
    markov->SetTransitions({t});

    markov->Run(1);
    auto results = markov->GetRunResults();

    EXPECT_TRUE(results[1].state.isApprox(Eigen::VectorXd::Constant(5, 2.0)));
}

TEST_F(MarkovTest, MultipleTransitions) {
    auto markov = Markov::Create("test_logger");
    markov->SetState(Eigen::VectorXd::Ones(5));

    transition t = {[](const Eigen::VectorXd &state,
                       const std::vector<Eigen::MatrixXd> &transitions,
                       HistoryStamp &hs) { return state + transitions[0]; },
                    {Eigen::VectorXd::Ones(5)}};
    markov->SetTransitions({t, t, t, t, t});

    markov->Run(1);
    auto results = markov->GetRunResults();

    EXPECT_TRUE(results[1].state.isApprox(Eigen::VectorXd::Constant(5, 6.0)));
}

TEST_F(MarkovTest, CopyConstructor) {
    auto markov = Markov::Create("test_logger");
    auto markov_two = markov->clone();
    EXPECT_EQ(markov_two->GetLoggerName(), "test_logger");
}

// TODO: Need to test the respond transition functions!
