cmake_minimum_required(VERSION 3.20)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED 
  COMPONENTS tokenizer)

add_library(data
  Loader.cpp
  CostLoader.cpp
  UtilityLoader.cpp
  DataLoader.cpp
  DataWriter.cpp
  DataFormatter.cpp
  DataTypes.cpp
  Configuration.cpp
)

target_include_directories(data PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${Eigen3_INCLUDE_DIRS}
  "include/"
)
target_link_libraries(data
  Eigen3::Eigen
  spdlog::spdlog
  boost_tokenizer
  utilities
)

install(TARGETS data
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
