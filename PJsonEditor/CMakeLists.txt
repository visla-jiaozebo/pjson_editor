

# Use FetchContent to get nlohmann_json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Fetch doctest for testing
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# Enable testing
enable_testing()

# Library target
add_library(pjson_editor 
    src/ControllerAPI.cpp
    src/DataStore.cpp
    src/ApiMessage.cpp
)

target_include_directories(pjson_editor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(pjson_editor
    PUBLIC
        nlohmann_json::nlohmann_json
)

# Set up the main target as an alias for easier CMake usage
add_library(PJsonEditor::pjson_editor ALIAS pjson_editor)

# Doctest based tests
add_executable(test_scene
    tests/test_scene.cpp
    tests/impl/server_login.cpp
)

target_link_libraries(test_scene
    PRIVATE
        pjson_editor
        doctest::doctest
)

# Register the test with CTest
add_test(NAME test_addscene_doctest COMMAND test_addscene_doctest)

# Install targets
install(TARGETS pjson_editor
    EXPORT PJsonEditorTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(DIRECTORY include/pjson_editor
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)