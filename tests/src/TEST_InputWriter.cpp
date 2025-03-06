////////////////////////////////////////////////////////////////////////////////
// File: TEST_InputWriter.cpp                                                 //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "MockDataLoader.hpp"
#include <respondsimulation/data_ops/Writer.hpp>

class InputWriterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InputWriterTest, ConstructorEmpty) {
    data_ops::InputWriter writer;
    std::string expected = "";
    EXPECT_EQ(writer.getDirname(), expected);
}

TEST_F(InputWriterTest, ConstructorStringAndVectorAndWriteType) {
    data_ops::InputWriter writer("testDirname", {1},
                                 data_ops::WriteType::STRING);
    EXPECT_EQ(writer.getDirname(), std::string("testDirname"));
}

// TEST(InputWriterTest, writeParameters) {
//     data_ops::InputWriter writer("testDirname", {1},
//                                   data_ops::WriteType::STRING);

//     std::shared_ptr<MockDataLoader> mockLoaderPtr =
//         std::make_shared<MockDataLoader>();

//     data_ops::Matrix3d mat3d(1, 1, 1);
//     mat3d(0, 0, 0) = 0.5;
//     EXPECT_CALL(*mockLoaderPtr, getOUDStates())
//         .WillRepeatedly(Return({"oud1"}));
//     EXPECT_CALL(*mockLoaderPtr, getOUDTransitionRates())
//         .WillRepeatedly(Return(mat3d));
//     EXPECT_CALL(*mockLoaderPtr, getNumOUDStates()).WillRepeatedly(Return(1));
//     EXPECT_CALL(*mockLoaderPtr, getNumInterventions())
//         .WillRepeatedly(Return(1));
//     EXPECT_CALL(*mockLoaderPtr, getDemographicCombos())
//         .WillRepeatedly(Return({"dc1"}));

//     writer.writeParameters(mockLoaderPtr);
// }

// TEST(SimpleWriteTest, SingleValueDimensions) {
//     data_ops::Matrix3d stateHistoryMatrix(2, 2, 2);
//     stateHistoryMatrix.setZero();
//     stateHistoryMatrix(1, 1, 1) = 1;

//     data_ops::Matrix3d overdoseHistoryMatrix(2, 2, 2);
//     overdoseHistoryMatrix.setZero();
//     overdoseHistoryMatrix(0, 0, 0) = 1;

//     data_ops::Matrix3d fatalOverdoseHistoryMatrix(2, 2, 2);
//     fatalOverdoseHistoryMatrix.setZero();

//     data_ops::Matrix3d mortalityHistoryMatrix(2, 2, 2);
//     mortalityHistoryMatrix.setZero();
//     mortalityHistoryMatrix(0, 0, 1) = 1;

//     std::vector<data_ops::Matrix3d> svec{stateHistoryMatrix};
//     std::vector<data_ops::Matrix3d> ovec{overdoseHistoryMatrix};
//     std::vector<data_ops::Matrix3d> fovec{fatalOverdoseHistoryMatrix};
//     std::vector<data_ops::Matrix3d> mvec{mortalityHistoryMatrix};

//     data_ops::Matrix4d stateHistory(svec);
//     data_ops::Matrix4d overdoseHistory(ovec);
//     data_ops::Matrix4d fatalOverdoseHistory(fovec);
//     data_ops::Matrix4d mortalityHistory(mvec);

//     data_ops::History history;
//     history.stateHistory = stateHistory;
//     history.overdoseHistory = overdoseHistory;
//     history.fatalOverdoseHistory = fatalOverdoseHistory;
//     history.mortalityHistory = mortalityHistory;

//     std::vector<std::string> interventions = {"inter1", "inter2"};
//     std::vector<std::string> oudStates = {"oud1", "oud2"};
//     std::vector<std::string> demographicCombos = {"10_14 Male", "10_14
//     Female"}; std::vector<std::string> demographics = {"agegrp", "sex"};
//     std::vector<int> timesteps = {1};

//     std::string dirname = "build/test/TestDir";

//     data_ops::HistoryWriter writer(dirname, interventions, oudStates,
//                                     demographics, demographicCombos,
//                                     timesteps, data_ops::STRING);
//     std::string result = writer.writeHistory(history);

//     std::string expected = "Interventions,OUD States,agegrp,sex,t+1,\n"
//                            "inter1,oud1,10_14,Male,0.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,\n"
//                            "inter2,oud2,10_14,Female,1.000000,\n"
//                            "Interventions,OUD States,agegrp,sex,t+1,\n"
//                            "inter1,oud1,10_14,Male,1.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,\n"
//                            "inter2,oud2,10_14,Female,0.000000,\n"
//                            "Interventions,OUD States,agegrp,sex,t+1,\n"
//                            "inter1,oud1,10_14,Male,0.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,\n"
//                            "inter2,oud2,10_14,Female,0.000000,\n"
//                            "Interventions,OUD States,agegrp,sex,t+1,\n"
//                            "inter1,oud1,10_14,Male,0.000000,\n"
//                            "inter1,oud1,10_14,Female,1.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,\n"
//                            "inter2,oud2,10_14,Female,0.000000,\n";

//     EXPECT_EQ(result, expected);
// }

// TEST(SimpleWriteTest, MultiTimestep) {
//     data_ops::Matrix3d stateHistoryMatrix1(2, 2, 2);
//     stateHistoryMatrix1.setZero();
//     stateHistoryMatrix1(0, 0, 0) = 1;
//     data_ops::Matrix3d stateHistoryMatrix2(2, 2, 2);
//     stateHistoryMatrix2.setZero();
//     stateHistoryMatrix2(1, 1, 1) = 1;

//     data_ops::Matrix3d overdoseHistoryMatrix1(2, 2, 2);
//     overdoseHistoryMatrix1.setZero();
//     overdoseHistoryMatrix1(1, 1, 1) = 1;
//     data_ops::Matrix3d overdoseHistoryMatrix2(2, 2, 2);
//     overdoseHistoryMatrix2.setZero();
//     overdoseHistoryMatrix2(0, 0, 0) = 1;

//     data_ops::Matrix3d fatalOverdoseHistoryMatrix1(2, 2, 2);
//     fatalOverdoseHistoryMatrix1.setZero();
//     data_ops::Matrix3d fatalOverdoseHistoryMatrix2(2, 2, 2);
//     fatalOverdoseHistoryMatrix2.setZero();

//     data_ops::Matrix3d mortalityHistoryMatrix1(2, 2, 2);
//     mortalityHistoryMatrix1.setZero();
//     mortalityHistoryMatrix1(1, 0, 0) = 1;
//     data_ops::Matrix3d mortalityHistoryMatrix2(2, 2, 2);
//     mortalityHistoryMatrix2.setZero();
//     mortalityHistoryMatrix2(0, 0, 1) = 1;

//     std::vector<data_ops::Matrix3d> svec{stateHistoryMatrix1,
//                                           stateHistoryMatrix2};
//     std::vector<data_ops::Matrix3d> ovec{overdoseHistoryMatrix1,
//                                           overdoseHistoryMatrix2};
//     std::vector<data_ops::Matrix3d> fovec{fatalOverdoseHistoryMatrix1,
//                                            fatalOverdoseHistoryMatrix2};
//     std::vector<data_ops::Matrix3d> mvec{mortalityHistoryMatrix1,
//                                           mortalityHistoryMatrix2};

//     data_ops::Matrix4d stateHistory(svec);
//     data_ops::Matrix4d overdoseHistory(ovec);
//     data_ops::Matrix4d fatalOverdoseHistory(fovec);
//     data_ops::Matrix4d mortalityHistory(mvec);

//     data_ops::History history;
//     history.stateHistory = stateHistory;
//     history.overdoseHistory = overdoseHistory;
//     history.fatalOverdoseHistory = fatalOverdoseHistory;
//     history.mortalityHistory = mortalityHistory;

//     std::vector<std::string> interventions = {"inter1", "inter2"};
//     std::vector<std::string> oudStates = {"oud1", "oud2"};
//     std::vector<std::string> demographicCombos = {"10_14 Male", "10_14
//     Female"}; std::vector<std::string> demographics = {"agegrp", "sex"};
//     std::vector<int> timesteps = {1, 2};

//     std::string dirname = "build/test/TestDir";

//     data_ops::HistoryWriter writer(dirname, interventions, oudStates,
//                                     demographics, demographicCombos,
//                                     timesteps, data_ops::STRING);
//     std::string result = writer.writeHistory(history);

//     std::string expected = "Interventions,OUD States,agegrp,sex,t+1,t+2,\n"
//                            "inter1,oud1,10_14,Male,1.000000,0.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Female,0.000000,1.000000,\n"
//                            "Interventions,OUD States,agegrp,sex,t+1,t+2,\n"
//                            "inter1,oud1,10_14,Male,0.000000,1.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Female,1.000000,0.000000,\n"
//                            "Interventions,OUD States,agegrp,sex,t+1,t+2,\n"
//                            "inter1,oud1,10_14,Male,0.000000,0.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Female,0.000000,0.000000,\n"
//                            "Interventions,OUD States,agegrp,sex,t+1,t+2,\n"
//                            "inter1,oud1,10_14,Male,0.000000,0.000000,\n"
//                            "inter1,oud1,10_14,Female,0.000000,1.000000,\n"
//                            "inter1,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter1,oud2,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Male,1.000000,0.000000,\n"
//                            "inter2,oud1,10_14,Female,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Male,0.000000,0.000000,\n"
//                            "inter2,oud2,10_14,Female,0.000000,0.000000,\n";

//     EXPECT_EQ(result, expected);
// }
