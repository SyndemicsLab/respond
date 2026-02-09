////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: respond                                                           //
// Created Date: 2026-02-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respond/simulation.hpp>

#include <memory>

#include <gtest/gtest.h>

#include "../mocks/transition_mock.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace respond {
namespace testing {
class SimulationTest : public ::testing::Test {
public:
    std::unique_ptr<Model> markov;
    Eigen::VectorXd state;

protected:
    void SetUp() override {
        markov = Model::Create("markov", "test_logger");
        state = Eigen::VectorXd(3);
        state << 1.0f, 2.0f, 3.0f;
    }
    void TearDown() override { markov.reset(); }
};

TEST_F(SimulationTest, Logger) {}
} // namespace testing
} // namespace respond