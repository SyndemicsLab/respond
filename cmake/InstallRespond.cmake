set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/respond")
set(export_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/respond")

#-------------------------------------------------------------------------------
# Include files
#-------------------------------------------------------------------------------
message(STATUS "Installing respond version ${RESPOND_VERSION} to ${CMAKE_INSTALL_FULL_LIBDIR} and ${CMAKE_INSTALL_FULL_INCLUDEDIR}...")

install(TARGETS respond_model
    EXPORT respondTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    FILE_SET HEADERS DESTINATION include
)

# respond_model is the full target and HEADERS is the list of header files
# It associates the target with the EXPORT definition which is then installed.
# install(
#     TARGETS respond_model
#     EXPORT respondTargets
#     FILE_SET HEADERS
# )

# This actually installs the exports to the specified destination and adds the
# namespace to the targets.
# install(
#     EXPORT respondTargets
#     DESTINATION ${export_cmake_dir}
#     NAMESPACE respond::
# )

include(CMakePackageConfigHelpers)

# Setup the Version config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfigVersion.cmake"
  VERSION ${RESPOND_VERSION} # Variable set in root CMakeLists.txt
  COMPATIBILITY AnyNewerVersion
)

# export the targets to a file and add the prefix
export(EXPORT respondTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/respond/respondTargets.cmake"
  NAMESPACE respond::
)

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/respondConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfig.cmake"
    @ONLY
)

set(ConfigPackageLocation lib/cmake/respond)
install(
    EXPORT respondTargets
    FILE respondTargets.cmake
    NAMESPACE respond::
    DESTINATION ${ConfigPackageLocation}
)


install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/respond/respondConfigVersion.cmake"
    DESTINATION ${export_cmake_dir}
)

#-------------------------------------------------------------------------------
# CPack Options for RESPOND
#-------------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/respondCPackOptions.cmake.in"
    "${PROJECT_BINARY_DIR}/respondCPackOptions.cmake"
    @ONLY
)
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/respondCPackOptions.cmake")

#-------------------------------------------------------------------------------
# Support creation of installable packages
#-------------------------------------------------------------------------------
include(CPack)
