cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(utils)
synmodels_extract_version()

project(synmodels VERSION ${SYNMODELS_VERSION} LANGUAGES CXX)

message(STATUS "Build synmodels: ${SYNMODELS_VERSION}")

include(GNUInstallDirs)

# ---------------------------------------------------------------------------------------
# Set default build to release
# ---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# ---------------------------------------------------------------------------------------
# Compiler config
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------------------
# Windows CXX Extensions
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

# ---------------------------------------------------------------------------------------
# Set SYNMODELS_MASTER_PROJECT to ON if we are building synmodels directly
# ---------------------------------------------------------------------------------------
if(NOT DEFINED SYNMODELS_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(SYNMODELS_MASTER_PROJECT ON)
    else()
        set(SYNMODELS_MASTER_PROJECT OFF)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# Set all default options
# ---------------------------------------------------------------------------------------
include(options)
LoadOptions()

# ---------------------------------------------------------------------------------------
# Set position independent code
# ---------------------------------------------------------------------------------------
if(SYNMODELS_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# ---------------------------------------------------------------------------------------
# Building the Library
# ---------------------------------------------------------------------------------------
set(SYNMODELS_SRCS 
    src/synmodels.cpp 
    src/data/EigenFactory.cpp
    src/data/RespondDataStore.cpp
    src/data/RespondDataStoreImpl.hpp
    src/kernels/StateTransitionModel.cpp
    src/kernels/StateTransitionModelImpl.hpp
    src/models/Respond.cpp
    src/models/RespondImpl.hpp
)
if(BUILD_SHARED_LIBS)
    if(WIN32)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
        list(APPEND SYNMODELS_SRCS ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
    add_library(synmodels SHARED ${SYNMODELS_SRCS} ${SYNMODELS_ALL_HEADERS})
    target_compile_definitions(synmodels PUBLIC SYNMODELS_SHARED_LIB)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(synmodels PUBLIC $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/wd4251
                                             /wd4275>)
    endif()
else()
    add_library(synmodels STATIC ${SYNMODELS_SRCS} ${SYNMODELS_ALL_HEADERS})
endif()

# ---------------------------------------------------------------------------------------
# Alias for importing
# ---------------------------------------------------------------------------------------
add_library(synmodels::synmodels ALIAS synmodels)

set(SYNMODELS_INCLUDES_LEVEL "")
if(SYNMODELS_SYSTEM_INCLUDES)
    set(SYNMODELS_INCLUDES_LEVEL "SYSTEM")
endif()

target_compile_definitions(synmodels PUBLIC SYNMODELS_COMPILED_LIB)
target_include_directories(synmodels ${SYNMODELS_INCLUDES_LEVEL} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

find_package(spdlog)
find_package(Eigen3)
                                                                  
target_link_libraries(synmodels
    PRIVATE
        Eigen3::Eigen
        spdlog::spdlog
)

set_target_properties(synmodels PROPERTIES VERSION ${SYNMODELS_VERSION} SOVERSION ${SYNMODELS_VERSION_MAJOR}.${SYNMODELS_VERSION_MINOR})

include(BuildBinaries)
BuildBinaries()

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
if(SYNMODELS_INSTALL)
    include(InstallSynmodels)
    InstallSynmodels()
endif()
