cmake_minimum_required(VERSION 3.27)
project(respond_tests LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to compile with, 17")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

if($ENV{FORCE_COLOR})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

if(PROJECT_IS_TOP_LEVEL OR NOT RESPOND_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but RESPOND_BUILD_TESTS is OFF!")
    return()
endif()

add_executable(respond_tests)

target_sources(respond_tests
    PRIVATE
        unit/background_test.cpp
        unit/behavior_test.cpp
        unit/cost_effectiveness_test.cpp
        unit/history_test.cpp
        unit/intervention_test.cpp
        unit/logging_test.cpp
        unit/markov_test.cpp
        unit/migration_test.cpp
        unit/overdose_test.cpp
        unit/simulation_test.cpp
        unit/timestep_test.cpp
        integration/respond_test.cpp
    )

target_include_directories(respond_tests
    PRIVATE
        "mocks/"
)

target_link_libraries(respond_tests 
PUBLIC 
    respond_model
    spdlog::spdlog
    GTest::gmock_main
    Eigen3::Eigen
)

gtest_discover_tests(respond_tests
    TEST_PREFIX "respond_"
)