cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(respondTest CXX)

include(FetchContent)

message(CHECK_START "Fetching GoogleTest")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "fetched")

message(CHECK_START "Fetching fmt")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "fetched")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME}
    src/RespondDataStoreTest.cpp
    src/StateTransitionModelTest.cpp
    src/RespondTest.cpp
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        GTest::gtest_main
        GTest::gmock_main
        fmt
        Eigen3::Eigen
        spdlog::spdlog
        kernels
        models
        data
        synmodels
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        "mocks/"
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})