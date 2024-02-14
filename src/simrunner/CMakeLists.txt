cmake_minimum_required(VERSION 3.20)

project(simrunner)

find_package (Eigen3 3.4 REQUIRED)

add_library(simrunner
    Simulation.cpp
    PostSimulationCalculator.cpp
)

target_link_libraries(simrunner PUBLIC Eigen3::Eigen)
target_link_libraries(simrunner PUBLIC matrixify)

target_include_directories(simrunner PUBLIC "include/")
