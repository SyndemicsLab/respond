cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(utils)
respond_extract_version()

project(respond VERSION ${RESPOND_VERSION} LANGUAGES CXX)

message(STATUS "Build respond: ${RESPOND_VERSION}")

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
# Set RESPOND_MASTER_PROJECT to ON if we are building respond directly
# ---------------------------------------------------------------------------------------
if(NOT DEFINED RESPOND_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(RESPOND_MASTER_PROJECT ON)
    else()
        set(RESPOND_MASTER_PROJECT OFF)
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
if(RESPOND_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# ---------------------------------------------------------------------------------------
# Building the Library
# ---------------------------------------------------------------------------------------
set(RESPOND_SRCS 
    src/respond.cpp 
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
        list(APPEND RESPOND_SRCS ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
    add_library(respond SHARED ${RESPOND_SRCS} ${RESPOND_ALL_HEADERS})
    target_compile_definitions(respond PUBLIC RESPOND_SHARED_LIB)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(respond PUBLIC $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/wd4251
                                             /wd4275>)
    endif()
else()
    add_library(respond STATIC ${RESPOND_SRCS} ${RESPOND_ALL_HEADERS})
endif()

# ---------------------------------------------------------------------------------------
# Alias for importing
# ---------------------------------------------------------------------------------------
add_library(respond::respond ALIAS respond)

set(RESPOND_INCLUDES_LEVEL "")
if(RESPOND_SYSTEM_INCLUDES)
    set(RESPOND_INCLUDES_LEVEL "SYSTEM")
endif()

target_compile_definitions(respond PUBLIC RESPOND_COMPILED_LIB)
target_include_directories(respond ${RESPOND_INCLUDES_LEVEL} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                                                                  PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>")

include(MakeDependenciesAvailable)
                                                                  
target_link_libraries(respond
    PRIVATE
        Eigen3::Eigen
        spdlog::spdlog
        datamanagement::datamanagement
)

set_target_properties(respond PROPERTIES VERSION ${RESPOND_VERSION} SOVERSION ${RESPOND_VERSION_MAJOR}.${RESPOND_VERSION_MINOR})

include(BuildBinaries)

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
if(RESPOND_INSTALL)
    include(InstallRespond)
endif()
