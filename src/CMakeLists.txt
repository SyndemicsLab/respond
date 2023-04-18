cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (RESPOND
	VERSION 2.0.0
	DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

include(GNUInstallDirs)

add_subdirectory(Model)
add_subdirectory(Controller)

add_executable(respond main.cpp)

target_link_libraries(respond
  Controller
  Model
)

# Create a simple configuration header
configure_file(config.h.in config.h)