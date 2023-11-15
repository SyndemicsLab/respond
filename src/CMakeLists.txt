cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (
  RESPOND
  VERSION 2.0.0
  DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

find_package(spdlog REQUIRED)

include(GNUInstallDirs)

add_subdirectory(model)
add_subdirectory(data)
add_subdirectory(utilities)

add_executable(respond main.cpp)

target_link_libraries(respond
  data
  model
  spdlog::spdlog
)

include(GNUInstallDirs)
install(TARGETS 
          respond
          model
          data
          utilities
        CONFIGURATIONS Debug
        RUNTIME
          DESTINATION Debug/bin
)

install(TARGETS 
          respond
          model
          data
          utilities
        CONFIGURATIONS Release
        RUNTIME
          DESTINATION Release/bin
)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "respond")
set(CPACK_PACKAGE_VENDOR "SyndemicsLab")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RESPOND - Opioid Use Disorder Population Based Simulation")
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_SOURCE_GENERATOR "ZIP")
include(CPack)
