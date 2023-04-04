cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

# Set some basic project attributes
project (RESPOND
	VERSION 0.1
	DESCRIPTION "A Compartmental Simulation for Opiod Use Disorder"
)

include(GNUInstallDirs)

add_library(mylib SHARED
	src/Simulation.cpp
)


include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

# This project will output an executable file
# add_executable(${PROJECT_NAME} RESPOND.cpp)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
# target_include_directories(${PROJECT_NAME} PUBLIC Simulation "${PROJECT_BINARY_DIR}")
