cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(utils)
respondsimulation_extract_version()

project(respondsimulation VERSION ${RESPONDSIMULATION_VERSION} LANGUAGES CXX)

message(STATUS "Build respondsimulation: ${RESPONDSIMULATION_VERSION}")

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
# Set RESPONDSIMULATION_MASTER_PROJECT to ON if we are building respondsimulation directly
# ---------------------------------------------------------------------------------------
if(NOT DEFINED RESPONDSIMULATION_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(RESPONDSIMULATION_MASTER_PROJECT ON)
    else()
        set(RESPONDSIMULATION_MASTER_PROJECT OFF)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# Set all default options
# ---------------------------------------------------------------------------------------
include(options)

include(LoadDataManagement)

# ---------------------------------------------------------------------------------------
# Set position independent code
# ---------------------------------------------------------------------------------------
if(RESPONDSIMULATION_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# ---------------------------------------------------------------------------------------
# Building the Library
# ---------------------------------------------------------------------------------------
set(RESPONDSIMULATION_SRCS 
    src/respondsimulation.cpp 
    src/data_ops/BaseLoader.cpp
    src/data_ops/CostLoader.cpp
    src/data_ops/DataFormatter.cpp
    src/data_ops/DataLoader.cpp
    src/data_ops/DataTypes.cpp
    src/data_ops/Matrix3dFactory.cpp
    src/data_ops/Matrix3dPrinter.cpp
    src/data_ops/UtilityLoader.cpp
    src/data_ops/Writer.cpp
    src/model/PostSimulationCalculator.cpp
    src/model/Simulation.cpp
)

if(BUILD_SHARED_LIBS)
    if(WIN32)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
        list(APPEND RESPONDSIMULATION_SRCS ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
    add_library(respondsimulation SHARED ${RESPONDSIMULATION_SRCS} ${RESPONDSIMULATION_ALL_HEADERS})
    target_compile_definitions(respondsimulation PUBLIC RESPONDSIMULATION_SHARED_LIB)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(respondsimulation PUBLIC $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/wd4251
                                             /wd4275>)
    endif()
else()
    add_library(respondsimulation STATIC ${RESPONDSIMULATION_SRCS} ${RESPONDSIMULATION_ALL_HEADERS})
endif()

# ---------------------------------------------------------------------------------------
# Alias for importing
# ---------------------------------------------------------------------------------------
add_library(respondsimulation::respondsimulation ALIAS respondsimulation)

set(RESPONDSIMULATION_INCLUDES_LEVEL "")
if(RESPONDSIMULATION_SYSTEM_INCLUDES)
    set(RESPONDSIMULATION_INCLUDES_LEVEL "SYSTEM")
endif()

target_compile_definitions(respondsimulation PUBLIC RESPONDSIMULATION_COMPILED_LIB)
target_include_directories(respondsimulation ${RESPONDSIMULATION_INCLUDES_LEVEL} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                                                                  PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>")

include(MakeDependenciesAvailable)
                                                                  
target_link_libraries(respondsimulation
    PRIVATE
        Eigen3::Eigen
        spdlog::spdlog
        datamanagement::datamanagement
)

set_target_properties(respondsimulation PROPERTIES VERSION ${RESPONDSIMULATION_VERSION} SOVERSION ${RESPONDSIMULATION_VERSION_MAJOR}.${RESPONDSIMULATION_VERSION_MINOR})

include(BuildBinaries)

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
if(RESPONDSIMULATION_INSTALL)
    include(InstallRespondsimulation)
endif()
