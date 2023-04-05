cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

# Set some basic project attributes
project (RESPOND
	VERSION 2.0.0
	DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

include(GNUInstallDirs)

add_library(respond SHARED
  src/Model/Simulation.cpp
  src/Controller/DataLoader.cpp
)

include_directories(include)

add_subdirectory(src/Model)
add_subdirectory(src/Controller)
add_subdirectory(test)

# This project will output an executable file
# add_executable(${PROJECT_NAME} RESPOND.cpp)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
# target_include_directories(${PROJECT_NAME} PUBLIC Simulation "${PROJECT_BINARY_DIR}")
