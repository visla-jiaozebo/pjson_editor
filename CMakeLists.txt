cmake_minimum_required(VERSION 3.20)
project(PJsonEditor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up installation paths
include(GNUInstallDirs)

add_subdirectory(PJsonEditor)

# Generate and install CMake config files
include(CMakePackageConfigHelpers)

# Generate the config file that includes the targets
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PJsonEditorConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PJsonEditorConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PJsonEditorConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PJsonEditor
)

# Install the config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PJsonEditorConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PJsonEditorConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PJsonEditor
)

