cmake_minimum_required(VERSION 3.10)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_library(Simulation
    Simulation.cpp
)

# Link Eigen
target_link_libraries (${PROJECT_NAME} Eigen3::Eigen)
