cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

# When both standard and multithreaded (mt) boost libraries exist, use standard
set(Boost_USE_MULTITHREADED OFF)

# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" ON)

# Only compile api if desired
option(BUILD_API "enable RESPOND api" OFF)

# used to fetch git repository dependencies
include(FetchContent)
# download fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)

# Set some basic project attributes
project (RESPONDREPO)

add_subdirectory(src)
if (BUILD_TESTS)
  # download gtest/gmock
  FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG 15460959cbbfa20e66ef0b5ab497367e47fc0a04
  )
  FetchContent_MakeAvailable(GTest)
  add_subdirectory(test)
endif()

if (BUILD_API)
  FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow
    GIT_TAG master
    FIND_PACKAGE_ARGS
  )
  FetchContent_MakeAvailable(Crow)
  add_subdirectory(api)
endif()
