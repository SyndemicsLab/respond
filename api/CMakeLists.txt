cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# Set some basic project attributes
project (api
	VERSION 2.0.0
	DESCRIPTION "API Exposure of RESPOND"
)

find_package(Crow)

add_executable(api main.cpp)

target_link_libraries(api
  Crow::Crow
  data
  model
)
