cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

# # Set some basic project attributes
# project (RESPONDSIM
# 	VERSION 0.1
# 	DESCRIPTION "A Compartmentalized Simulation for Opiod Use Disorder"
# )

# Find OpenMP
find_package(OpenMP REQUIRED)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_library(Simulation
    Simulation.cpp
)

# Link Eigen
if(WIN32)
target_link_libraries (Simulation Eigen3)
endif(WIN32)

if(UNIX)
target_link_libraries(Simulation Eigen3::Eigen)
endif(UNIX)

target_link_libraries(Simulation OpenMP::OpenMP_CXX)
