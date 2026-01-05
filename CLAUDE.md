# idris2-wasm

Idris2 → WebAssembly compilation pipeline for ICP canisters.

## Strategy

```
Idris2 (.idr) → RefC backend → C code → Emscripten → WASM
```

## Build

```bash
# 1. Generate C from Idris2
idris2 --codegen refc -o main.c Main.idr

# 2. Compile C to WASM with Emscripten
emcc main.c -o main.wasm
```

## References

- [Emscripten](https://emscripten.org/docs/compiling/WebAssembly.html)
- [Idris2 RefC backend](https://github.com/idris-lang/Idris2)
