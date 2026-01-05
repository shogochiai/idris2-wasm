# idris2-wasm

Idris2 to WebAssembly compilation pipeline via RefC backend + Emscripten.

## Pipeline

```
Idris2 (.idr) → RefC backend → C code → Emscripten → WASM
```

## Status

| Step | Status | Notes |
|------|--------|-------|
| Idris2 → C | ✅ Working | RefC backend generates C code |
| C → Native | ✅ Working | Output: "Hello from Idris2 WASM!" |
| C → WASM | ⚠️ Compiles | emscripten 3.1.6 has output issues |

**Known Issue**: Ubuntu's emscripten 3.1.6 has compatibility issues with the Idris2 RefC runtime. WASM compiles but doesn't produce stdout output. Recommended: Install latest emscripten via [emsdk](https://emscripten.org/docs/getting_started/downloads.html).

## Prerequisites

- [Idris2](https://github.com/idris-lang/Idris2) with RefC backend
- [Emscripten](https://emscripten.org/) (recommend latest via emsdk)

## Quick Start

```bash
# Build hello example to WASM
./scripts/build-wasm.sh hello

# Output: build/hello/main.js (with embedded WASM)
node build/hello/main.js
```

## Project Structure

```
idris2-wasm/
├── examples/
│   └── hello/Main.idr    # Hello World example
├── scripts/
│   └── build-wasm.sh     # Build pipeline script
├── build/                # Generated output (gitignored)
└── README.md
```

## How It Works

1. **Idris2 → C**: Uses `--codegen refc` to generate C code
2. **C → WASM**: Emscripten compiles C with:
   - RefC runtime sources (downloaded from Idris2 repo)
   - mini-gmp for arbitrary precision integers
   - WASM output in single-file JS mode

## Dependencies Downloaded Automatically

- `mini-gmp.c/h` - Minimal GMP implementation for WASM
- RefC runtime sources from Idris2 repository

## Native Build (for testing)

```bash
# Compile to native executable for comparison
gcc build/hello/exec/main.c \
    /tmp/refc-src/*.c \
    -I$IDRIS2_PREFIX/idris2-*/support/refc \
    -I$IDRIS2_PREFIX/idris2-*/support/c \
    -lgmp -o main_native

./main_native
# Output: Hello from Idris2 WASM!
```

## Roadmap

- [x] Pure Idris2 → WASM pipeline
- [ ] IC0 canister imports for ICP
- [ ] dfx deployment support
