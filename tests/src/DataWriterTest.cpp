//===-- DataWriterTest.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:33 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "DataWriter.hpp"

using namespace Matrixify;

TEST(DataWriterCreation, DefaultConstructor) {
    DataWriter writer;
    std::string expected = "";
    EXPECT_EQ(writer.getDirname(), expected); // comparing two std::strings
}

TEST(DataWriterCreation, MainConstructor) {
    std::string dirname = "testDir";
    std::vector<std::string> interventions;
    std::vector<std::string> oudStates;
    std::vector<std::string> demo;
    std::vector<std::string> demographics;
    std::vector<std::string> demographicCombos;
    std::vector<int> timesteps;
    bool stateOutput;
    History history;

    DataWriter writer(dirname, interventions, oudStates, demographics,
                      demographicCombos, timesteps, stateOutput);
    EXPECT_EQ(writer.getDirname(), dirname);
}

TEST(SimpleWriteTest, SingleValueDimensions) {
    Matrix3d stateHistoryMatrix(2, 2, 2);
    stateHistoryMatrix.setZero();
    stateHistoryMatrix(1, 1, 1) = 1;

    Matrix3d overdoseHistoryMatrix(2, 2, 2);
    overdoseHistoryMatrix.setZero();
    overdoseHistoryMatrix(0, 0, 0) = 1;

    Matrix3d fatalOverdoseHistoryMatrix(2, 2, 2);
    fatalOverdoseHistoryMatrix.setZero();

    Matrix3d mortalityHistoryMatrix(2, 2, 2);
    mortalityHistoryMatrix.setZero();
    mortalityHistoryMatrix(0, 0, 1) = 1;

    std::vector<Matrix3d> svec{stateHistoryMatrix};
    std::vector<Matrix3d> ovec{overdoseHistoryMatrix};
    std::vector<Matrix3d> fovec{fatalOverdoseHistoryMatrix};
    std::vector<Matrix3d> mvec{mortalityHistoryMatrix};

    Matrix4d stateHistory(svec);
    Matrix4d overdoseHistory(ovec);
    Matrix4d fatalOverdoseHistory(fovec);
    Matrix4d mortalityHistory(mvec);

    History history;
    history.stateHistory = stateHistory;
    history.overdoseHistory = overdoseHistory;
    history.fatalOverdoseHistory = fatalOverdoseHistory;
    history.mortalityHistory = mortalityHistory;

    std::vector<std::string> interventions = {"inter1", "inter2"};
    std::vector<std::string> oudStates = {"oud1", "oud2"};
    std::vector<std::string> demographicCombos = {"10_14 Male", "10_14 Female"};
    std::vector<std::string> demographics = {"agegrp", "sex"};
    std::vector<int> timesteps = {0};

    std::string dirname = "build/test/TestDir";

    DataWriter writer(dirname, interventions, oudStates, demographics,
                      demographicCombos, timesteps, true);
    std::string result = writer.writeHistory(STRING, history);

    std::string expected = "Interventions, OUD States,agegrp,sex,t+0,\n"
                           "inter1,oud1,10_14,Male,0.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,\n"
                           "inter2,oud2,10_14,Female,1.000000,\n"
                           "Interventions, OUD States,agegrp,sex,t+0,\n"
                           "inter1,oud1,10_14,Male,1.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,\n"
                           "inter2,oud2,10_14,Female,0.000000,\n"
                           "Interventions, OUD States,agegrp,sex,t+0,\n"
                           "inter1,oud1,10_14,Male,0.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,\n"
                           "inter2,oud2,10_14,Female,0.000000,\n"
                           "Interventions, OUD States,agegrp,sex,t+0,\n"
                           "inter1,oud1,10_14,Male,0.000000,\n"
                           "inter1,oud1,10_14,Female,1.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,\n"
                           "inter2,oud2,10_14,Female,0.000000,\n";

    EXPECT_EQ(result, expected);
}

TEST(SimpleWriteTest, MultiTimestep) {
    Matrix3d stateHistoryMatrix1(2, 2, 2);
    stateHistoryMatrix1.setZero();
    stateHistoryMatrix1(0, 0, 0) = 1;
    Matrix3d stateHistoryMatrix2(2, 2, 2);
    stateHistoryMatrix2.setZero();
    stateHistoryMatrix2(1, 1, 1) = 1;

    Matrix3d overdoseHistoryMatrix1(2, 2, 2);
    overdoseHistoryMatrix1.setZero();
    overdoseHistoryMatrix1(1, 1, 1) = 1;
    Matrix3d overdoseHistoryMatrix2(2, 2, 2);
    overdoseHistoryMatrix2.setZero();
    overdoseHistoryMatrix2(0, 0, 0) = 1;

    Matrix3d fatalOverdoseHistoryMatrix1(2, 2, 2);
    fatalOverdoseHistoryMatrix1.setZero();
    Matrix3d fatalOverdoseHistoryMatrix2(2, 2, 2);
    fatalOverdoseHistoryMatrix2.setZero();

    Matrix3d mortalityHistoryMatrix1(2, 2, 2);
    mortalityHistoryMatrix1.setZero();
    mortalityHistoryMatrix1(1, 0, 0) = 1;
    Matrix3d mortalityHistoryMatrix2(2, 2, 2);
    mortalityHistoryMatrix2.setZero();
    mortalityHistoryMatrix2(0, 0, 1) = 1;

    std::vector<Matrix3d> svec{stateHistoryMatrix1, stateHistoryMatrix2};
    std::vector<Matrix3d> ovec{overdoseHistoryMatrix1, overdoseHistoryMatrix2};
    std::vector<Matrix3d> fovec{fatalOverdoseHistoryMatrix1,
                                fatalOverdoseHistoryMatrix2};
    std::vector<Matrix3d> mvec{mortalityHistoryMatrix1,
                               mortalityHistoryMatrix2};

    Matrix4d stateHistory(svec);
    Matrix4d overdoseHistory(ovec);
    Matrix4d fatalOverdoseHistory(fovec);
    Matrix4d mortalityHistory(mvec);

    History history;
    history.stateHistory = stateHistory;
    history.overdoseHistory = overdoseHistory;
    history.fatalOverdoseHistory = fatalOverdoseHistory;
    history.mortalityHistory = mortalityHistory;

    std::vector<std::string> interventions = {"inter1", "inter2"};
    std::vector<std::string> oudStates = {"oud1", "oud2"};
    std::vector<std::string> demographicCombos = {"10_14 Male", "10_14 Female"};
    std::vector<std::string> demographics = {"agegrp", "sex"};
    std::vector<int> timesteps = {0, 1};

    std::string dirname = "build/test/TestDir";

    DataWriter writer(dirname, interventions, oudStates, demographics,
                      demographicCombos, timesteps, true);
    std::string result = writer.writeHistory(Matrixify::STRING, history);

    std::string expected = "Interventions, OUD States,agegrp,sex,t+0,t+1,\n"
                           "inter1,oud1,10_14,Male,1.000000,0.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Female,0.000000,1.000000,\n"
                           "Interventions, OUD States,agegrp,sex,t+0,t+1,\n"
                           "inter1,oud1,10_14,Male,0.000000,1.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Female,1.000000,0.000000,\n"
                           "Interventions, OUD States,agegrp,sex,t+0,t+1,\n"
                           "inter1,oud1,10_14,Male,0.000000,0.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Female,0.000000,0.000000,\n"
                           "Interventions, OUD States,agegrp,sex,t+0,t+1,\n"
                           "inter1,oud1,10_14,Male,0.000000,0.000000,\n"
                           "inter1,oud1,10_14,Female,0.000000,1.000000,\n"
                           "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud1,10_14,Male,1.000000,0.000000,\n"
                           "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
                           "inter2,oud2,10_14,Female,0.000000,0.000000,\n";

    EXPECT_EQ(result, expected);
}
