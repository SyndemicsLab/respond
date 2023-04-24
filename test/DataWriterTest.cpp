#include <gtest/gtest.h>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include "DataWriter.hpp"

using namespace Data;

TEST(DataWriterCreation, DefaultConstructor){
    DataWriter writer;
    std::string expected = "";
    EXPECT_EQ(writer.getDirname(), expected); // comparing two std::strings
}

TEST(DataWriterCreation, MainConstructor){
    std::string dirname = "testDir";
    std::vector<std::string> interventions;
    std::vector<std::string> oudStates;
    std::vector<std::string> demo;
    std::vector<std::vector<std::string>> demographics;
    History history;

    DataWriter writer(dirname, interventions, oudStates, demographics, history);
    EXPECT_EQ(writer.getDirname(), dirname);
}

TEST(SimpleWriteTest, SingleValueDimensions){
    Matrix3d stateHistoryMatrix(2,2,2);
    stateHistoryMatrix.setZero();
    stateHistoryMatrix(1,1,1) = 1;

    Matrix3d overdoseHistoryMatrix(2,2,2);
    overdoseHistoryMatrix.setZero();
    overdoseHistoryMatrix(0,0,0) = 1;

    Matrix3d mortalityHistoryMatrix(2,2,2);
    mortalityHistoryMatrix.setZero();
    mortalityHistoryMatrix(0,0,1) = 1;

    std::vector<Matrix3d> svec{stateHistoryMatrix};
    std::vector<Matrix3d> ovec{overdoseHistoryMatrix};
    std::vector<Matrix3d> mvec{mortalityHistoryMatrix};

    Matrix3dOverTime stateHistory(svec);
    Matrix3dOverTime overdoseHistory(ovec);
    Matrix3dOverTime mortalityHistory(mvec);

    History history;
    history.stateHistory = stateHistory;
    history.overdoseHistory = overdoseHistory;
    history.mortalityHistory = mortalityHistory;
    
    std::vector<std::string> interventions = {"inter1", "inter2"};
    std::vector<std::string> oudStates = {"oud1", "oud2"};
    std::vector<std::string> demo1 = {"10_14", "Male"};
    std::vector<std::string> demo2 = {"10_14", "Female"}; 
    std::vector<std::vector<std::string>> demographics = {demo1, demo2};
    
    std::string dirname = "build/test/TestDir";

    DataWriter writer(dirname, interventions, oudStates, demographics, history);
    std::string result = writer.Write(STRING);

    std::string expected = "Interventions, OUD States,Demographic 0,Demographic 1,t+0,\n"
    "inter1,oud1,10_14,Male,0,\n"
    "inter1,oud1,10_14,Female,0,\n"
    "inter1,oud2,10_14,Male,0,\n"
    "inter1,oud2,10_14,Female,0,\n"
    "inter2,oud1,10_14,Male,0,\n"
    "inter2,oud1,10_14,Female,0,\n"
    "inter2,oud2,10_14,Male,0,\n"
    "inter2,oud2,10_14,Female,1,\n"
    "Interventions, OUD States,Demographic 0,Demographic 1,t+0,\n"
    "inter1,oud1,10_14,Male,1,\n"
    "inter1,oud1,10_14,Female,0,\n"
    "inter1,oud2,10_14,Male,0,\n"
    "inter1,oud2,10_14,Female,0,\n"
    "inter2,oud1,10_14,Male,0,\n"
    "inter2,oud1,10_14,Female,0,\n"
    "inter2,oud2,10_14,Male,0,\n"
    "inter2,oud2,10_14,Female,0,\n"
    "Interventions, OUD States,Demographic 0,Demographic 1,t+0,\n"
    "inter1,oud1,10_14,Male,0,\n"
    "inter1,oud1,10_14,Female,1,\n"
    "inter1,oud2,10_14,Male,0,\n"
    "inter1,oud2,10_14,Female,0,\n"
    "inter2,oud1,10_14,Male,0,\n"
    "inter2,oud1,10_14,Female,0,\n"
    "inter2,oud2,10_14,Male,0,\n"
    "inter2,oud2,10_14,Female,0,\n";

    EXPECT_EQ(result, expected);    
}

TEST(SimpleWriteTest, MultiTimestep){
    Matrix3d stateHistoryMatrix1(2,2,2);
    stateHistoryMatrix1.setZero();
    stateHistoryMatrix1(0,0,0) = 1;
    Matrix3d stateHistoryMatrix2(2,2,2);
    stateHistoryMatrix2.setZero();
    stateHistoryMatrix2(1,1,1) = 1;

    Matrix3d overdoseHistoryMatrix1(2,2,2);
    overdoseHistoryMatrix1.setZero();
    overdoseHistoryMatrix1(1,1,1) = 1;
    Matrix3d overdoseHistoryMatrix2(2,2,2);
    overdoseHistoryMatrix2.setZero();
    overdoseHistoryMatrix2(0,0,0) = 1;

    Matrix3d mortalityHistoryMatrix1(2,2,2);
    mortalityHistoryMatrix1.setZero();
    mortalityHistoryMatrix1(1,0,0) = 1;
    Matrix3d mortalityHistoryMatrix2(2,2,2);
    mortalityHistoryMatrix2.setZero();
    mortalityHistoryMatrix2(0,0,1) = 1;

    std::vector<Matrix3d> svec{stateHistoryMatrix1, stateHistoryMatrix2};
    std::vector<Matrix3d> ovec{overdoseHistoryMatrix1, overdoseHistoryMatrix2};
    std::vector<Matrix3d> mvec{mortalityHistoryMatrix1, mortalityHistoryMatrix2};

    Matrix3dOverTime stateHistory(svec);
    Matrix3dOverTime overdoseHistory(ovec);
    Matrix3dOverTime mortalityHistory(mvec);

    History history;
    history.stateHistory = stateHistory;
    history.overdoseHistory = overdoseHistory;
    history.mortalityHistory = mortalityHistory;
    
    std::vector<std::string> interventions = {"inter1", "inter2"};
    std::vector<std::string> oudStates = {"oud1", "oud2"};
    std::vector<std::string> demo1 = {"10_14", "Male"};
    std::vector<std::string> demo2 = {"10_14", "Female"}; 
    std::vector<std::vector<std::string>> demographics = {demo1, demo2};
    
    std::string dirname = "build/test/TestDir";

    DataWriter writer(dirname, interventions, oudStates, demographics, history);
    std::string result = writer.Write(Data::STRING);

    std::string expected = "Interventions, OUD States,Demographic 0,Demographic 1,t+0,t+1,\n"
    "inter1,oud1,10_14,Male,1,0,\n"
    "inter1,oud1,10_14,Female,0,0,\n"
    "inter1,oud2,10_14,Male,0,0,\n"
    "inter1,oud2,10_14,Female,0,0,\n"
    "inter2,oud1,10_14,Male,0,0,\n"
    "inter2,oud1,10_14,Female,0,0,\n"
    "inter2,oud2,10_14,Male,0,0,\n"
    "inter2,oud2,10_14,Female,0,1,\n"
    "Interventions, OUD States,Demographic 0,Demographic 1,t+0,t+1,\n"
    "inter1,oud1,10_14,Male,0,1,\n"
    "inter1,oud1,10_14,Female,0,0,\n"
    "inter1,oud2,10_14,Male,0,0,\n"
    "inter1,oud2,10_14,Female,0,0,\n"
    "inter2,oud1,10_14,Male,0,0,\n"
    "inter2,oud1,10_14,Female,0,0,\n"
    "inter2,oud2,10_14,Male,0,0,\n"
    "inter2,oud2,10_14,Female,1,0,\n"
    "Interventions, OUD States,Demographic 0,Demographic 1,t+0,t+1,\n"
    "inter1,oud1,10_14,Male,0,0,\n"
    "inter1,oud1,10_14,Female,0,1,\n"
    "inter1,oud2,10_14,Male,0,0,\n"
    "inter1,oud2,10_14,Female,0,0,\n"
    "inter2,oud1,10_14,Male,1,0,\n"
    "inter2,oud1,10_14,Female,0,0,\n"
    "inter2,oud2,10_14,Male,0,0,\n"
    "inter2,oud2,10_14,Female,0,0,\n";

    EXPECT_EQ(result, expected);    
}

