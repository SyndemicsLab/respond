cmake_minimum_required(VERSION 3.19)
set (CMAKE_CXX_STANDARD 20)

# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" ON)

# Only compile tests if desired
option(BUILD_API "enable RESPOND api" ON)

# Set some basic project attributes
project (RESPONDREPO)

add_subdirectory(src)
if (BUILD_TESTS)
  add_subdirectory(test)
endif()

if (BUILD_API)
  add_subdirectory(api)
endif()
