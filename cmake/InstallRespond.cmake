message(STATUS "Installing respond version ${RESPOND_VERSION}...")

set(project_config "${CMAKE_CURRENT_LIST_DIR}/respondConfig.cmake")
set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/cmake/respond")
set(version_config_file "${CMAKE_CURRENT_BINARY_DIR}/respondConfigVersion.cmake")

set(pkgconfig_install_dir "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
set(pkg_config "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc")

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
    DESTINATION ${export_dest_dir}
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
    DESTINATION ${export_dest_dir}
)

#-------------------------------------------------------------------------------
# Install pkg-config file
#-------------------------------------------------------------------------------
# if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
#     set(PKG_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
# else()
#     set(PKG_CONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
# endif()
# if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
#     set(PKG_CONFIG_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
# else()
#     set(PKG_CONFIG_LIBDIR "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
# endif()
# get_target_property(PKG_CONFIG_DEFINES respond INTERFACE_COMPILE_DEFINITIONS)
# string(REPLACE ";" " -D" PKG_CONFIG_DEFINES "${PKG_CONFIG_DEFINES}")
# string(CONCAT PKG_CONFIG_DEFINES "-D" "${PKG_CONFIG_DEFINES}")
# configure_file("cmake/${PROJECT_NAME}.pc.in" "${pkg_config}" @ONLY)
# install(FILES "${pkg_config}" DESTINATION "${pkgconfig_install_dir}")


#-------------------------------------------------------------------------------
# Support creation of installable packages
#-------------------------------------------------------------------------------
# include(cmake/respondCPack.cmake)
