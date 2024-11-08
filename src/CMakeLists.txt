cmake_minimum_required(VERSION 3.20)

# Set some basic project attributes
project (
  respond
  VERSION 2.0.0
  DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_MakeAvailable(spdlog)

add_subdirectory(data)
add_subdirectory(model)
add_subdirectory(simulation)

option(BUILD_API "enable RESPOND API" OFF)
if(BUILD_API STREQUAL ON)
  add_subdirectory(api)
endif()

option(BUILD_PYBINDINGS "enable RESPOND Python Bindings" OFF)
if (NOT BUILD_PYBINDINGS STREQUAL OFF)
  add_subdirectory(pybindings)
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "data/include"
        "model/include"
        "simulation/include"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        data
        model
        simulation
        spdlog::spdlog
)
