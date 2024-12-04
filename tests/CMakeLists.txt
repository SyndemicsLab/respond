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

message(CHECK_START "Fetching Eigen3")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# note: To disable eigen tests,
# you should put this code in a add_subdirectory to avoid to change
# BUILD_TESTING for your own project too since variables are directory
# scoped
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
FetchContent_MakeAvailable(Eigen)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "fetched")

message(CHECK_START "Fetching spdlog")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "fetched")

find_package(DataManagement 1 REQUIRED
    HINTS "${CMAKE_SOURCE_DIR}/bin/dminstall"
)

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
        DataManagement::DataManagement
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        "mocks/"
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})