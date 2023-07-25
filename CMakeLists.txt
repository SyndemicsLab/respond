cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (RESPONDREPO)

# When both standard and multithreaded (mt) boost libraries exist, use standard
set(Boost_USE_MULTITHREADED OFF)

# Only compile api if desired
option(BUILD_API "enable RESPOND api" OFF)

add_subdirectory(src)

find_package(fmt REQUIRED)

# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" ON)

if (BUILD_TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(test)
endif()

if (BUILD_API)
  find_package(crow REQUIRED)
  add_subdirectory(api)
endif()
