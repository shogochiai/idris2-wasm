#!/bin/bash
# Build Idris2 to WASM via RefC + Emscripten
set -e

EXAMPLE="${1:-hello}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
EXAMPLE_DIR="$PROJECT_DIR/examples/$EXAMPLE"
BUILD_DIR="$PROJECT_DIR/build/$EXAMPLE"

echo "=== Building $EXAMPLE to WASM ==="

# Check dependencies
command -v idris2 >/dev/null 2>&1 || { echo "idris2 not found"; exit 1; }
command -v emcc >/dev/null 2>&1 || { echo "emcc not found. Install: https://emscripten.org/docs/getting_started/downloads.html"; exit 1; }

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ">>> Step 1: Idris2 → C (RefC backend)"
idris2 --codegen refc --output-dir "$BUILD_DIR" -o main "$EXAMPLE_DIR/Main.idr"

echo ">>> Step 2: C → WASM (Emscripten)"
# Find generated C file
C_FILE=$(find "$BUILD_DIR" -name "*.c" | head -1)
if [ -z "$C_FILE" ]; then
    echo "No C file generated"
    exit 1
fi

emcc "$C_FILE" \
    -o "$BUILD_DIR/main.wasm" \
    -s STANDALONE_WASM=1 \
    -s EXPORTED_FUNCTIONS='["_main"]' \
    --no-entry

echo ">>> Done!"
echo "Output: $BUILD_DIR/main.wasm"
ls -la "$BUILD_DIR"/*.wasm
