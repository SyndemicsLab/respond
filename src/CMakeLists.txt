cmake_minimum_required(VERSION 3.10)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
# Find SQLite3
find_package(SQLite3 REQUIRED)

add_library(Simulation
    Simulation.cpp
)

# Link Eigen
if(WIN32)
target_link_libraries (${PROJECT_NAME} Eigen3)
target_link_libraries(${PROJECT_NAME} SQLite3)
endif(WIN32)

if(UNIX)
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} SQLite3)
endif(UNIX)
