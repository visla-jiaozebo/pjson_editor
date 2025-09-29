

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
)

target_include_directories(pjson_editor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(pjson_editor
    PUBLIC
        $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
)

# Set up the main target as an alias for easier CMake usage
add_library(PJsonEditor::pjson_editor ALIAS pjson_editor)

# Test executables
add_executable(test_extended_models
    tests/test_extended_models.cpp
)

target_link_libraries(test_extended_models
    PRIVATE
        pjson_editor
)

add_executable(test_tier1_apis
    tests/test_tier1_apis.cpp
)

target_link_libraries(test_tier1_apis
    PRIVATE
        pjson_editor
)

add_executable(test_business_logic
    tests/test_business_logic.cpp
)

target_link_libraries(test_business_logic
    PRIVATE
        pjson_editor
)

add_executable(test_merge_scenes_logic
    tests/test_merge_scenes_logic.cpp
)

target_link_libraries(test_merge_scenes_logic
    PRIVATE
        pjson_editor
)

add_executable(test_merge_simple
    tests/test_merge_simple.cpp
)

target_link_libraries(test_merge_simple
    PRIVATE
        pjson_editor
)

add_executable(test_all_operations
    tests/test_all_operations.cpp
)

target_link_libraries(test_all_operations
    PRIVATE
        pjson_editor
)

# Doctest based tests
add_executable(test_addscene_doctest
    tests/test_addscene_doctest.cpp
)

target_link_libraries(test_addscene_doctest
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

install(EXPORT PJsonEditorTargets
    FILE PJsonEditorTargets.cmake
    NAMESPACE PJsonEditor::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PJsonEditor
)
