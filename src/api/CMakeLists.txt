cmake_minimum_required(VERSION 3.19)
set (CMAKE_CXX_STANDARD 20)
find_package(Crow)

# Set some basic project attributes
project (api
	VERSION 2.0.0
	DESCRIPTION "API Exposure of RESPOND"
)

add_executable(${PROJECT_NAME} 
    main.cpp
    api.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Crow::Crow)
target_link_libraries(${PROJECT_NAME} PUBLIC matrixify)
target_link_libraries(${PROJECT_NAME} PUBLIC simulation)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
