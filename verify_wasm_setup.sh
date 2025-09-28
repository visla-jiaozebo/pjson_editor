#!/bin/bash

# WebAssembly Compilation Verification Script
# This script checks if all necessary components are in place for WASM compilation

echo "=== WebAssembly Implementation Verification ==="
echo

# Check current directory structure
echo "1. Checking directory structure..."
PROJECT_ROOT="/Users/zzchu/Desktop/workspace/visla-api/PJsonEditor"

if [ ! -d "$PROJECT_ROOT" ]; then
    echo "❌ Project root not found: $PROJECT_ROOT"
    exit 1
fi

cd "$PROJECT_ROOT"

# Check essential files
echo "2. Checking essential files..."
FILES_TO_CHECK=(
    "CMakeLists.wasm.txt"
    "wasm_bindings.cpp"
    "build_wasm.sh"
    "demo/index.html"
    "PJsonEditor/include/pjson_editor/ExtendedAPI.h"
    "PJsonEditor/include/pjson_editor/ExtendedModels.h"
)

ALL_FILES_EXIST=true
for file in "${FILES_TO_CHECK[@]}"; do
    if [ -f "$file" ]; then
        echo "✅ $file"
    else
        echo "❌ Missing: $file"
        ALL_FILES_EXIST=false
    fi
done

# Check Emscripten availability
echo
echo "3. Checking Emscripten SDK..."
if command -v emcc &> /dev/null; then
    echo "✅ Emscripten compiler found: $(emcc --version | head -1)"
    EMSCRIPTEN_AVAILABLE=true
else
    echo "⚠️  Emscripten not found in PATH"
    echo "   To install: https://emscripten.org/docs/getting_started/downloads.html"
    EMSCRIPTEN_AVAILABLE=false
fi

# Check CMake
echo
echo "4. Checking CMake..."
if command -v cmake &> /dev/null; then
    echo "✅ CMake found: $(cmake --version | head -1)"
    CMAKE_AVAILABLE=true
else
    echo "❌ CMake not found"
    CMAKE_AVAILABLE=false
fi

# Summary and next steps
echo
echo "=== Verification Summary ==="

if [ "$ALL_FILES_EXIST" = true ]; then
    echo "✅ All WebAssembly implementation files are in place"
else
    echo "❌ Some files are missing"
fi

if [ "$CMAKE_AVAILABLE" = true ]; then
    echo "✅ CMake is available"
else
    echo "❌ CMake is required"
fi

if [ "$EMSCRIPTEN_AVAILABLE" = true ]; then
    echo "✅ Emscripten SDK is available"
    echo
    echo "🚀 Ready to compile WebAssembly!"
    echo "Next steps:"
    echo "   1. cd $PROJECT_ROOT"
    echo "   2. chmod +x build_wasm.sh"
    echo "   3. ./build_wasm.sh"
    echo "   4. Open demo/index.html in a web browser"
else
    echo "⚠️  Emscripten SDK needs to be installed"
    echo
    echo "📋 Installation steps:"
    echo "   1. git clone https://github.com/emscripten-core/emsdk.git"
    echo "   2. cd emsdk"
    echo "   3. ./emsdk install latest"
    echo "   4. ./emsdk activate latest"
    echo "   5. source ./emsdk_env.sh"
    echo "   6. Return to this project and run ./build_wasm.sh"
fi

echo
echo "📁 Project structure ready for WebAssembly deployment"
echo "📖 See WebAssembly_Implementation_Guide.md for detailed instructions"
