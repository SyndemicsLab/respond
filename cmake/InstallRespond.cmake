message(STATUS "Installing respond version ${RESPOND_VERSION}...")

set(project_config "${CMAKE_CURRENT_LIST_DIR}/respondConfig.cmake")
set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/respond")
set(export_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/respond")
set(version_config_file "${CMAKE_CURRENT_BINARY_DIR}/respondConfigVersion.cmake")

#-------------------------------------------------------------------------------
# Include files
#-------------------------------------------------------------------------------
message("Installing respond to ${CMAKE_INSTALL_FULL_LIBDIR} and ${CMAKE_INSTALL_FULL_INCLUDEDIR}...")

install(
    TARGETS respond_model
    EXPORT respondTargets
    FILE_SET HEADERS
)

install(
    EXPORT respondTargets
    DESTINATION ${export_cmake_dir}
    NAMESPACE respond::
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${version_config_file}
  COMPATIBILITY ExactVersion
)

install(
    FILES
        ${project_config}
        ${version_config_file}
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
