# idris2-wasm

Idris2 to WebAssembly compilation pipeline via RefC backend + Emscripten.

## Pipeline

```
Idris2 (.idr) → RefC backend → C code → Emscripten → WASM → WASI stub → IC Deploy
```

## Status

| Step | Status | Notes |
|------|--------|-------|
| Idris2 → C | ✅ Working | RefC backend generates C code |
| C → Native | ✅ Working | Output: "Hello from Idris2 WASM!" |
| C → WASM | ✅ Working | emsdk 4.0.22 + STANDALONE_WASM |
| WASI stubbing | ✅ Working | tools/stub-wasi.sh removes WASI imports |
| IC deployment | ✅ Working | Canister deployed and responding |

## Prerequisites

- [Idris2](https://github.com/idris-lang/Idris2) with RefC backend
- [Emscripten](https://emscripten.org/) via emsdk (4.0.22+)
- [dfx](https://internetcomputer.org/docs/current/developer-docs/setup/install) (for IC deployment)
- [wabt](https://github.com/WebAssembly/wabt) (for WASI stubbing)

## Quick Start

### ICP Canister Deployment

```bash
# 1. Build canister WASM
./scripts/build-canister.sh

# 2. Stub WASI imports (required for IC)
./tools/stub-wasi.sh build/canister/canister.wasm build/canister/canister_stubbed.wasm

# 3. Deploy to local IC
dfx start --background
dfx deploy idris2_canister

# 4. Call methods
dfx canister call idris2_canister greet
# Returns: "Hello from Idris2 on IC!" (raw bytes)

dfx canister call idris2_canister ping
# Returns: "pong" (raw bytes)
```

## Project Structure

```
idris2-wasm/
├── examples/
│   ├── hello/Main.idr        # Hello World example
│   └── canister/Main.idr     # ICP canister example
├── scripts/
│   ├── build-wasm.sh         # Build WASM for Node.js
│   └── build-canister.sh     # Build WASM for ICP canisters
├── tools/
│   └── stub-wasi.sh          # WASI import stubber
├── support/
│   └── ic0/
│       ├── ic0.h             # IC0 system API declarations
│       └── canister_entry.c  # Canister entry point wrappers
├── docs/
│   └── research-blockers.md  # Research notes on WASI blockers
├── dfx.json                  # dfx project config
├── canister.did              # Candid interface
└── build/                    # Generated output (gitignored)
```

## ICP Canister Build

The canister build produces standalone WASM with:

**Exports (IC convention with spaces):**
- `canister_init` - Called on canister initialization
- `canister_query greet` - Query method (note: space not underscore)
- `canister_update ping` - Update method

**Imports (from IC runtime):**
- `ic0.debug_print` - Debug logging
- `ic0.msg_reply` - Send reply
- `ic0.msg_reply_data_append` - Append data to reply

**Build flags:**
```bash
emcc ... -s STANDALONE_WASM=1 -s FILESYSTEM=0 -s ERROR_ON_UNDEFINED_SYMBOLS=0
```

## WASI Stubbing

Emscripten produces WASM with WASI imports that IC doesn't support:
- `wasi_snapshot_preview1.fd_close`
- `wasi_snapshot_preview1.fd_write`
- `wasi_snapshot_preview1.fd_seek`

The `tools/stub-wasi.sh` script:
1. Converts WASM to WAT (text format)
2. Replaces WASI imports with stub functions returning 0
3. Converts back to WASM

Result: Clean WASM with only `ic0.*` imports.

## How It Works

1. **Idris2 → C**: Uses `--codegen refc` to generate C code
2. **C → WASM**: Emscripten compiles with:
   - `STANDALONE_WASM=1` for pure WASM output
   - RefC runtime sources (downloaded from Idris2 repo)
   - mini-gmp for arbitrary precision integers
   - IC0 entry points with `export_name` attributes
3. **WASI stubbing**: Replace unsupported imports with stubs
4. **IC deploy**: Deploy to local or mainnet IC

## Dependencies Downloaded Automatically

- `mini-gmp.c/h` - Minimal GMP implementation for WASM
- RefC runtime sources from Idris2 repository

## Roadmap

- [x] Pure Idris2 → WASM pipeline
- [x] IC0 canister imports for ICP
- [x] dfx project configuration
- [x] WASI import stubbing
- [x] IC deployment support
- [ ] Candid encoding for proper dfx integration
- [ ] Mainnet deployment

## Known Limitations

- Returns raw bytes instead of Candid-encoded data (dfx shows hex)
- Requires manual WASI stubbing step (not integrated into build yet)
- Limited IC0 API surface (msg_reply, debug_print)
