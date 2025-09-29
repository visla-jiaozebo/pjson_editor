cmake_minimum_required(VERSION 3.20)
project(PJsonEditorExample)

set(CMAKE_CXX_STANDARD 17)

# Find required dependencies with status checking
find_package(nlohmann_json 3.11.3 QUIET)
if(nlohmann_json_FOUND)
    message(STATUS "Found nlohmann_json: ${nlohmann_json_VERSION}")
else()
    message(FATAL_ERROR "nlohmann_json not found")
endif()

find_package(PJsonEditor QUIET)
if(PJsonEditor_FOUND)
    message(STATUS "Found PJsonEditor: ${PJsonEditor_VERSION}")
    message(STATUS "PJsonEditor install prefix: ${PJsonEditor_DIR}")
else()
    message(FATAL_ERROR "PJsonEditor not found")
endif()

# You can also check for specific targets
if(TARGET PJsonEditor::pjson_editor)
    message(STATUS "PJsonEditor::pjson_editor target is available")
else()
    message(FATAL_ERROR "PJsonEditor::pjson_editor target not found")
endif()

# Create an example executable
add_executable(example main.cpp)

# Link against the installed library and its dependencies
target_link_libraries(example 
    PRIVATE 
        PJsonEditor::pjson_editor
        nlohmann_json::nlohmann_json
)