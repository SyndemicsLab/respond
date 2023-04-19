cmake_minimum_required(VERSION 3.19)
set (CMAKE_CXX_STANDARD 20)

# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" ON)

# Set some basic project attributes
project (RESPONDREPO)

add_subdirectory(src)
if (BUILD_TESTS)
  add_subdirectory(test)
endif()
