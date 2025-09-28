#!/bin/bash

# WebAssembly Build Script for PJsonEditor
# This script builds the PJsonEditor library for WebAssembly using Emscripten

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🚀 Building PJsonEditor for WebAssembly${NC}"

# Check if Emscripten is installed
if ! command -v emcc &> /dev/null; then
    echo -e "${RED}❌ Error: Emscripten (emcc) not found${NC}"
    echo "Please install Emscripten SDK:"
    echo "  1. git clone https://github.com/emscripten-core/emsdk.git"
    echo "  2. cd emsdk"
    echo "  3. ./emsdk install latest"
    echo "  4. ./emsdk activate latest"
    echo "  5. source ./emsdk_env.sh"
    exit 1
fi

# Create build directory for WebAssembly
WASM_BUILD_DIR="build_wasm"
echo -e "${YELLOW}📁 Creating WebAssembly build directory: ${WASM_BUILD_DIR}${NC}"

if [ -d "$WASM_BUILD_DIR" ]; then
    rm -rf "$WASM_BUILD_DIR"
fi
mkdir -p "$WASM_BUILD_DIR"

cd "$WASM_BUILD_DIR"

# Configure CMake for WebAssembly using WebAssembly-specific CMakeLists
echo "⚙️  Configuring CMake for WebAssembly..."
echo "configure: cmake -f ../CMakeLists.wasm.txt ../PJsonEditor -DCMAKE_BUILD_TYPE=Release -DEMSCRIPTEN=1 $CMAKE_TOOLCHAIN_FLAG $CMAKE_EMULATOR_FLAG"

# Copy WebAssembly CMakeLists to the source directory temporarily
cp ../CMakeLists.wasm.txt ../PJsonEditor/CMakeLists.txt.wasm

# Use the WebAssembly CMakeLists
emcmake cmake -DCMAKE_CURRENT_SOURCE_DIR=../PJsonEditor -DCMAKE_BUILD_TYPE=Release -DEMSCRIPTEN=1 $CMAKE_TOOLCHAIN_FLAG $CMAKE_EMULATOR_FLAG -DCMAKE_BINARY_DIR=$(pwd) .

# Create a temporary CMakeLists.txt in current directory
cat > CMakeLists.txt << 'WASM_CMAKE_EOF'
cmake_minimum_required(VERSION 3.20)
project(PJsonEditor_WASM VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emscripten-specific settings
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    
    # Emscripten compiler flags
    set(EMSCRIPTEN_COMPILE_FLAGS
        -O3
        -s WASM=1
        -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']
        -s ALLOW_MEMORY_GROWTH=1
        -s INITIAL_MEMORY=33554432
        -s MAXIMUM_MEMORY=134217728
        -s STACK_SIZE=1048576
        -s MODULARIZE=1
        -s EXPORT_NAME='PJsonEditor'
        -s ENVIRONMENT='web,worker'
        --bind
    )
    
    # Convert list to string
    string(JOIN " " EMSCRIPTEN_FLAGS_STR ${EMSCRIPTEN_COMPILE_FLAGS})
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS_STR}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_FLAGS_STR}")
endif()

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
    ../PJsonEditor/src/ControllerAPI.cpp
    ../PJsonEditor/src/DataStore.cpp
)

target_include_directories(pjson_editor
    PUBLIC
        ../PJsonEditor/include
)

target_link_libraries(pjson_editor
    PUBLIC
        nlohmann_json::nlohmann_json
)

# WebAssembly API bindings
add_executable(pjson_wasm
    ../wasm_bindings.cpp
)

target_link_libraries(pjson_wasm
    PRIVATE
        pjson_editor
)

# Set additional Emscripten-specific properties
set_target_properties(pjson_wasm PROPERTIES
    LINK_FLAGS "--bind -s EXPORTED_FUNCTIONS=['_malloc','_free']"
)
WASM_CMAKE_EOF

emcmake cmake . -DCMAKE_BUILD_TYPE=Release -DEMSCRIPTEN=1 $CMAKE_TOOLCHAIN_FLAG $CMAKE_EMULATOR_FLAG

if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed"
    exit 1
fi

# Build
echo -e "${YELLOW}🔨 Building...${NC}"
emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ WebAssembly build completed successfully!${NC}"
    echo ""
    echo "Generated files:"
    ls -la *.js *.wasm 2>/dev/null || echo "No WASM files found (normal for library builds)"
    
    # Copy files to demo directory
    DEMO_DIR="../demo"
    mkdir -p "$DEMO_DIR"
    
    # Copy any generated files
    find . -name "*.js" -exec cp {} "$DEMO_DIR/" \;
    find . -name "*.wasm" -exec cp {} "$DEMO_DIR/" \;
    
    echo -e "${GREEN}📦 Files copied to demo directory${NC}"
else
    echo -e "${RED}❌ Build failed${NC}"
    exit 1
fi

cd ..

echo -e "${GREEN}🎉 Build process completed!${NC}"
echo ""
echo "Next steps:"
echo "  1. cd demo"
echo "  2. python3 -m http.server 8000"
echo "  3. Open http://localhost:8000 in your browser"
