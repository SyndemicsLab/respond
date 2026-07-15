cmake_minimum_required(VERSION 3.27)
set(PRIVATE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${PRIVATE_MODULE_PATH}/utils.cmake)
respond_extract_version()

#-------------------------------------------------------------------------------
# Compiler config
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-------------------------------------------------------------------------------
# Project Setup
#-------------------------------------------------------------------------------
project(respond VERSION ${RESPOND_VERSION} LANGUAGES CXX)
message(STATUS "Build respond: ${RESPOND_VERSION}")

#-------------------------------------------------------------------------------
# Make all targets build to top level folders in build directory
#-------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#-------------------------------------------------------------------------------
# Set default build to release
#-------------------------------------------------------------------------------
# This is a standard recipe for setting a default build type
set(_default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${_default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${_default_build_type}"
        CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#-------------------------------------------------------------------------------
# Windows CXX Extensions
#-------------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

#-------------------------------------------------------------------------------
# Set RESPOND_MASTER_PROJECT to ON if we are building respond directly
#-------------------------------------------------------------------------------
if(NOT DEFINED RESPOND_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(RESPOND_MASTER_PROJECT ON)
    else()
        set(RESPOND_MASTER_PROJECT OFF)
    endif()
endif()

#-------------------------------------------------------------------------------
# Set all default options
#-------------------------------------------------------------------------------
include(${PRIVATE_MODULE_PATH}/options.cmake)

#-------------------------------------------------------------------------------
# Set position independent code
#-------------------------------------------------------------------------------
if(RESPOND_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

#-------------------------------------------------------------------------------
# Building the Library
#-------------------------------------------------------------------------------
add_library(respond_model)
add_library(respond::respond_model ALIAS respond_model)

target_sources(respond_model
    PRIVATE
        src/internals/background.hpp
        src/internals/behavior.hpp
        src/internals/intervention.hpp
        src/internals/logging_internals.hpp
        src/internals/markov.hpp
        src/internals/migration.hpp
        src/internals/overdose.hpp
        src/internals/transition_base.hpp
        src/background.cpp
        src/behavior.cpp
        src/intervention.cpp
        src/logging.cpp
        src/migration.cpp
        src/model_factory.cpp
        src/overdose.cpp
        src/transition_factory.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/respond/constants.hpp
            include/respond/cost_effectiveness.hpp
            include/respond/history.hpp
            include/respond/logging.hpp
            include/respond/model.hpp
            include/respond/respond.hpp
            include/respond/simulation.hpp
            include/respond/timestep.hpp
            include/respond/transition.hpp
            include/respond/version.hpp
    )

#-------------------------------------------------------------------------------
# Linking Libraries
#-------------------------------------------------------------------------------
include(${PRIVATE_MODULE_PATH}/MakeDependenciesAvailable.cmake)
set(private_deps spdlog::spdlog)
if (RESPOND_CALCULATE_COVERAGE)
    list(APPEND private_deps gcov)
endif()

target_link_libraries(respond_model
PUBLIC
    Eigen3::Eigen
PRIVATE
    ${private_deps}
)

#-------------------------------------------------------------------------------
# Setting library details
#-------------------------------------------------------------------------------
set_target_properties(respond_model PROPERTIES VERSION ${RESPOND_VERSION} SOVERSION ${RESPOND_VERSION_MAJOR}.${RESPOND_VERSION_MINOR})

include(${PRIVATE_MODULE_PATH}/BuildBinaries.cmake)

#-------------------------------------------------------------------------------
# Install
#-------------------------------------------------------------------------------
include(GNUInstallDirs)
if(RESPOND_INSTALL)
    include(${PRIVATE_MODULE_PATH}/InstallRespond.cmake)
endif()
