cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

# Set some basic project attributes
project (RESPOND
	VERSION 0.1
	DESCRIPTION "A Compartmentalized Simulation for Opiod Use Disorder"
)


add_subdirectory(src)

# include_directories is needed for the compiler to know where looking for Eigen3 header files to be included 
# include_directories( ${EIGEN3_INCLUDE_DIR}/include )

# This project will output an executable file
add_executable(${PROJECT_NAME} RESPOND.cpp)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC Simulation "${PROJECT_BINARY_DIR}")
