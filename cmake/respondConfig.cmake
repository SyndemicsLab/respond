include(CMakeFindDependencyMacro)

include("${CMAKE_CURRENT_LIST_DIR}/respondTargets.cmake")
find_dependency(Eigen3 REQUIRED)
find_dependency(spdlog REQUIRED)
find_dependency(GTest QUIET) # Only if tests are built