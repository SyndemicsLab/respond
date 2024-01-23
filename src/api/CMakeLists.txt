cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (api
	VERSION 2.0.0
	DESCRIPTION "API Exposure of RESPOND"
)

add_executable(api main.cpp)

target_link_libraries(api
  Crow::Crow
  matrixify
  model
)
