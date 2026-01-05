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
| C → WASM (hello) | ⚠️ Compiles | emscripten 3.1.6 has output issues |
| C → WASM (canister) | ⚠️ Compiles | WASI imports block IC deployment |

**Known Issues**:
- Ubuntu's emscripten 3.1.6 has compatibility issues with Idris2 RefC runtime
- Canister WASM includes WASI/env imports (`fd_write`, `abort`, etc.) that IC doesn't support
- Recommend using latest emscripten via [emsdk](https://emscripten.org/docs/getting_started/downloads.html)

## Prerequisites

- [Idris2](https://github.com/idris-lang/Idris2) with RefC backend
- [Emscripten](https://emscripten.org/) (recommend latest via emsdk)
- [dfx](https://internetcomputer.org/docs/current/developer-docs/setup/install) (for IC deployment)

## Quick Start

### Hello World (WASM for Node.js)

```bash
./scripts/build-wasm.sh hello
node build/hello/main.js
```

### ICP Canister (Standalone WASM)

```bash
./scripts/build-canister.sh
# Output: build/canister/canister.wasm (22KB)
```

## Project Structure

```
idris2-wasm/
├── examples/
│   ├── hello/Main.idr      # Hello World example
│   └── canister/Main.idr   # ICP canister example
├── scripts/
│   ├── build-wasm.sh       # Build WASM for Node.js
│   └── build-canister.sh   # Build WASM for ICP canisters
├── support/
│   └── ic0/
│       ├── ic0.h           # IC0 system API declarations
│       └── canister_entry.c # Canister entry point wrappers
├── dfx.json                # dfx project config
├── canister.did            # Candid interface
├── build/                  # Generated output (gitignored)
└── README.md
```

## ICP Canister Build

The canister build produces standalone WASM with:

**Exports:**
- `canister_init` - Called on canister initialization
- `canister_query_greet` - Query method
- `canister_update_ping` - Update method

**Imports (from IC runtime):**
- `ic0.debug_print` - Debug logging
- `ic0.msg_reply` - Send reply
- `ic0.msg_reply_data_append` - Append data to reply

**Known Issue:** Emscripten 3.1.6 also adds WASI imports (`wasi_snapshot_preview1.*`) and env imports (`env.abort`, `env.emscripten_*`) that the IC doesn't support. These need to be stubbed or removed for IC deployment.

```bash
# Build canister
./scripts/build-canister.sh

# Inspect WASM structure
wasm-objdump -x build/canister/canister.wasm

# Inspect with ic-wasm
ic-wasm build/canister/canister.wasm info
```

## Deploying to IC (WIP)

```bash
# Start local replica
dfx start --background

# Deploy (currently fails due to WASI imports)
dfx deploy idris2_canister

# Call canister methods (after successful deployment)
dfx canister call idris2_canister greet
dfx canister call idris2_canister ping
```

## How It Works

1. **Idris2 → C**: Uses `--codegen refc` to generate C code
2. **C → WASM**: Emscripten compiles C with:
   - RefC runtime sources (downloaded from Idris2 repo)
   - mini-gmp for arbitrary precision integers
   - IC0 entry points for canister interface

## Dependencies Downloaded Automatically

- `mini-gmp.c/h` - Minimal GMP implementation for WASM
- RefC runtime sources from Idris2 repository

## Native Build (for testing)

```bash
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
- [x] IC0 canister imports for ICP
- [x] dfx project configuration
- [ ] Remove WASI/env imports for IC compatibility
- [ ] Full dfx deployment support
