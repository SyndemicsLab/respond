cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

# When both standard and multithreaded (mt) boost libraries exist, use standard
set(Boost_USE_MULTITHREADED OFF)

# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" OFF)

# Only compile api if desired
# option(WITH_API "enable RESPOND api" OFF)

# Set some basic project attributes
project (RESPONDREPO)

add_subdirectory(src)

if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()

if (WITH_API)
  add_subdirectory(api)
endif()


