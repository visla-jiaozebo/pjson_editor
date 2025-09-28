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

# Library target
add_library(pjson_editor 
    src/ControllerAPI.cpp
    src/DataStore.cpp
)

target_include_directories(pjson_editor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pjson_editor
    PUBLIC
        nlohmann_json::nlohmann_json
)

# CLI executable (optional)
add_executable(pjson_cli
    src/main_extended.cpp
)

target_link_libraries(pjson_cli
    PRIVATE
        pjson_editor
)

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
