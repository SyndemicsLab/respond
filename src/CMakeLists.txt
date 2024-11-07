cmake_minimum_required(VERSION 3.20)

# Set some basic project attributes
project (
  respond
  VERSION 2.0.0
  DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

add_subdirectory(simulation)
add_subdirectory(matrixify)

option(BUILD_API "enable RESPOND API" OFF)
if(BUILD_API STREQUAL ON)
  add_subdirectory(api)
endif()

option(BUILD_PYBINDINGS "enable RESPOND Python Bindings" OFF)
if (NOT BUILD_PYBINDINGS STREQUAL OFF)
  add_subdirectory(pybindings)
endif()

add_executable(respond main.cpp)

target_include_directories(respond PRIVATE
  "matrixify/include"
  "simulation/include"
  )

target_link_libraries(respond
  matrixify
  simulation
  spdlog::spdlog
)
