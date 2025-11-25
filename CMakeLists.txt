cmake_minimum_required(VERSION 3.27)
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(PRIVATE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${PRIVATE_MODULE_PATH}/utils.cmake)
respond_extract_version()

project(respond VERSION ${RESPOND_VERSION} LANGUAGES CXX)

message(STATUS "Build respond: ${RESPOND_VERSION}")

#-------------------------------------------------------------------------------
# Set default build to release
#-------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

#-------------------------------------------------------------------------------
# Compiler config
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        src/markov.cpp
        src/logging.cpp
        src/internals/logging_internals.hpp
        src/internals/markov_internals.hpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/respond/cost_effectiveness.hpp
            include/respond/helpers.hpp
            include/respond/logging.hpp
            include/respond/markov.hpp
            include/respond/respond.hpp
            include/respond/types.hpp
            include/respond/version.hpp
    )

#-------------------------------------------------------------------------------
# Windows and MSVC specific settings
#-------------------------------------------------------------------------------
if(WIN32)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
    list(APPEND RESPOND_SRCS ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(respond_model PUBLIC $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/wd4251/wd4275>)
endif()

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

set_target_properties(respond_model PROPERTIES VERSION ${RESPOND_VERSION} SOVERSION ${RESPOND_VERSION_MAJOR}.${RESPOND_VERSION_MINOR})

include(${PRIVATE_MODULE_PATH}/BuildBinaries.cmake)

#-------------------------------------------------------------------------------
# Install
#-------------------------------------------------------------------------------
include(GNUInstallDirs)
if(RESPOND_INSTALL)
    include(${PRIVATE_MODULE_PATH}/InstallRespond.cmake)
endif()
