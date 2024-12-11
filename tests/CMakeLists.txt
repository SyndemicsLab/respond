cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(respondTest CXX)

include(LoadFmt)
LoadFmt()

include(LoadGoogleTest)
LoadGoogleTest()

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

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