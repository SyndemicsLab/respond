cmake_minimum_required(VERSION 3.20)

find_package(spdlog REQUIRED)

include(GNUInstallDirs)

# Set some basic project attributes
project (
  respond
  VERSION 2.0.0
  DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

add_subdirectory(model)
add_subdirectory(data)
add_subdirectory(utilities)

option(BUILD_API "enable RESPOND API" OFF)
if(NOT BUILD_API STREQUAL OFF)
  add_subdirectory(api)
endif()

add_executable(respond main.cpp)

include_directories(
  "${CMAKE_SOURCE_DIR}/src/data/include"
  "${CMAKE_SOURCE_DIR}/src/model/include"
  "${CMAKE_SOURCE_DIR}/src/utilities/include"
  )

target_link_libraries(respond
  data
  model
  utilities
  spdlog::spdlog
)

install(TARGETS 
          respond
        RUNTIME
          DESTINATION bin
)
