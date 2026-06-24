message(STATUS "Installing respond version ${RESPOND_VERSION} to ${CMAKE_INSTALL_FULL_LIBDIR} and ${CMAKE_INSTALL_FULL_INCLUDEDIR}...")

# This defines where things get installed. The default is to install to the
# system level directories (e.g. /usr/lib, /usr/respond/include). We will want
# to change this to allow for user level install too via the
# CMAKE_INSTALL_PREFIX variable.
install(TARGETS respond_model
    EXPORT respondTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    FILE_SET HEADERS DESTINATION include
)

include(CMakePackageConfigHelpers)

# Setup the Version config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfigVersion.cmake"
  VERSION ${RESPOND_VERSION} # Variable set in root CMakeLists.txt
  COMPATIBILITY AnyNewerVersion
)

# export the targets to a file and add the prefix. This file is used by the
# config file to find the compiled targets during `find_package(respond)` calls.
export(EXPORT respondTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/respond/respondTargets.cmake"
  NAMESPACE respond::
)

# Creates the config file necessary for CMake to use `find_package(respond)` # after being installed.
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/respondConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfig.cmake"
    @ONLY
)

# Define where to install the config files and install the targets before the 
# config files. This is so that the config files can actually find the targets
set(ConfigPackageLocation lib/cmake/respond)
install(
    EXPORT respondTargets
    FILE respondTargets.cmake
    NAMESPACE respond::
    DESTINATION ${ConfigPackageLocation}
)

# Finally, install the config files
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/respond"
)

#-------------------------------------------------------------------------------
# CPack: This helps us create various installers. All options are controlled in
# the `respondCPackOptions.cmake.in` config file. Please do not change CPack
# itself, only the Options file!
#-------------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/respondCPackOptions.cmake.in"
    "${PROJECT_BINARY_DIR}/respondCPackOptions.cmake"
    @ONLY
)
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/respondCPackOptions.cmake")

include(CPack)
