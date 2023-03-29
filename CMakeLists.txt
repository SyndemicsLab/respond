cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 11)

# Set some basic project attributes
project (RESPOND
	VERSION 0.1
	DESCRIPTION "A Hello World Project")
	
# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

# This project will output an executable file
add_executable(${PROJECT_NAME} RESPOND.cpp)

# Link Eigen
target_link_libraries (${PROJECT_NAME} Eigen3::Eigen)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
