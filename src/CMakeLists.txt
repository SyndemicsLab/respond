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
add_subdirectory(kernels)
add_subdirectory(models)

option(BUILD_API "enable RESPOND API" OFF)
if(BUILD_API STREQUAL ON)
  add_subdirectory(api)
endif()

option(BUILD_PYBINDINGS "enable RESPOND Python Bindings" OFF)
if (NOT BUILD_PYBINDINGS STREQUAL OFF)
  add_subdirectory(pybindings)
endif()


option(BUILD_SHARED_LIBS "Build Using Shared Libraries" ON)
if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED main.cpp)
else()
    add_library(${PROJECT_NAME} STATIC main.cpp)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "data/include"
        "kernels/include"
        "simulation/include"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        data
        kernel
        models
        spdlog::spdlog
)
