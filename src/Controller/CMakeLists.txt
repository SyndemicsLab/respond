cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

add_library(Controller
  DataLoader.cpp
)

# Link Boost
if(UNIX)
  find_package(Boost REQUIRED COMPONENTS
    filesystem)
  target_link_libraries(Controller
    Boost::filesystem
  )
endif(UNIX)
