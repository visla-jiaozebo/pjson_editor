cmake_minimum_required(VERSION 3.20)
project(PJsonEditor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
# enable_testing()

# Library target
add_library(pjson_editor
    src/ControllerAPI.cpp
    src/DataStore.cpp
    src/ApiMessage.cpp
    src/pjson_editor.cpp
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

# Emscripten-specific settings
if(EMSCRIPTEN)
    message(STATUS "Configuring for Emscripten - Simple WASM build")

    # Simple WASM executable without glue code
    add_executable(pjson_editor_wasm
        src/wasm_bindings_simple.cpp
    )

    target_link_libraries(pjson_editor_wasm
        PRIVATE
        pjson_editor
    )

    # Minimal Emscripten flags for simple WASM build
    set_target_properties(pjson_editor_wasm PROPERTIES
        SUFFIX ".js"
        COMPILE_FLAGS "-std=c++17 -O2 -s WASM=1"
        LINK_FLAGS "-std=c++17 -O2 -s WASM=1 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] --bind -s ALLOW_MEMORY_GROWTH=1"
    )
else()
    add_subdirectory(tests)
endif()

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
