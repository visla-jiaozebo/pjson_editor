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

# Configure with Emscripten
echo -e "${YELLOW}⚙️  Configuring CMake for WebAssembly...${NC}"
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DEMSCRIPTEN=1

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
