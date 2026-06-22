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

file(GLOB RESPOND_TEST_MOCKS mocks/*.hpp)
file(GLOB RESPOND_UNIT_TESTS CONFIGURE_DEPENDS unit/*.cpp)
file(GLOB RESPOND_INTEGRATION_TESTS CONFIGURE_DEPENDS integration/*.cpp)

add_executable(respond_tests)

target_sources(respond_tests
    PRIVATE
        ${RESPOND_UNIT_TESTS}
        ${RESPOND_INTEGRATION_TESTS}
    )

target_include_directories(respond_tests
    PRIVATE
        "mocks/"
)

target_link_libraries(respond_tests 
PUBLIC 
    respond_model
    Eigen3::Eigen
    spdlog::spdlog
    GTest::gmock_main
)

gtest_discover_tests(respond_tests
    TEST_PREFIX "respond_"
)