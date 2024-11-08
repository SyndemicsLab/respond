cmake_minimum_required(VERSION 3.20)

project(matrixify)

include(FetchContent)
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

find_package(DataManagement 1 REQUIRED
    HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()



target_sources(${PROJECT_NAME}
    PRIVATE
        Tensor3dFactory.cpp
        RespondDataBlock.cpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        spdlog::spdlog 
        DataManagement::DataManagement
    PUBLIC
        Eigen3::Eigen
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
