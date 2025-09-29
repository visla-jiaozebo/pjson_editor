#!/bin/bash

# Simplified WebAssembly build script
echo "🚀 Building PJsonEditor WebAssembly with Bindings"

PROJECT_ROOT="/Users/zzchu/Desktop/workspace/pjson_editor"
cd "$PROJECT_ROOT"

# Clean build directory
echo "🧹 Cleaning build directory..."
rm -rf build_wasm
mkdir -p build_wasm
cd build_wasm

echo "📦 Building library and WebAssembly module directly..."

# Build the library first
echo "Step 1: Building PJsonEditor library..."
emcc -O3 -I../PJsonEditor/include \
    -I../PJsonEditor/build/_deps/nlohmann_json-src/include \
    ../PJsonEditor/src/ControllerAPI.cpp \
    ../PJsonEditor/src/DataStore.cpp \
    -c

# Build WebAssembly module with bindings
echo "Step 2: Building WebAssembly module with JavaScript bindings..."
emcc -O3 -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='PJsonEditor' \
    -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=33554432 \
    -s ENVIRONMENT='web,worker' \
    --bind \
    -I../PJsonEditor/include \
    -I../PJsonEditor/build/_deps/nlohmann_json-src/include \
    ../PJsonEditor/src/ControllerAPI.cpp \
    ../PJsonEditor/src/DataStore.cpp \
    ../wasm_bindings.cpp \
    -o pjson_editor_web.js

if [ $? -eq 0 ]; then
    echo "✅ WebAssembly build successful!"
    echo "Generated files:"
    ls -la pjson_editor_web.*
    
    # Copy to demo directory
    echo "📁 Copying files to demo directory..."
    cp pjson_editor_web.js ../demo/
    cp pjson_editor_web.wasm ../demo/
    
    echo "🎉 Build completed!"
    echo "Next steps:"
    echo "  1. cd ../demo"
    echo "  2. python3 -m http.server 8000" 
    echo "  3. Open http://localhost:8000 in your browser"
else
    echo "❌ Build failed"
    exit 1
fi
