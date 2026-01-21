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

## Source Map Generation

ビルド時に `build/idris2-c.map` を生成。C行番号→Idrisソースファイル/関数名のマッピング。

```json
{
  "names": ["Main.canisterUpdate", "Main.computeSum", "PrimIO.unsafePerformIO", ...],
  "sources": ["Main.idr", "Prelude/IO.idr", ...]
}
```

カバレッジ計算の分母（テスト対象関数一覧）として使用可能。

## ic-wasm Instrumentation

ICP canister向けにprofilingを追加する場合：

```bash
# 1. WASI stubbing済みWASMを生成
idris2-wasm build --canister=xxx

# 2. ic-wasmでinstrument
# 重要: --start-page 10 でcanisterのstable memory (0-9) と衝突回避
ic-wasm build/xxx_stubbed.wasm -o build/xxx_instrumented.wasm instrument \
  --start-page 10 --page-limit 16
```

**重要**: ic-wasmは以下の関数を追加：
- `__get_profiling` - トレースデータ取得（query）
- `__toggle_tracing` - トレース有効/無効切替（update）※呼ぶとトレース無効化！
- `__toggle_entry` - ログ保持モード切替（update）
- `__get_cycles` - サイクルカウント取得（query）

### Stable Memory Pre-allocation for Profiling

ic-wasmはstable memoryにトレースを書き込む。canisterが独自にstable memoryを使う場合、
衝突を避けるため`canister_init`で事前確保が必要：

```c
// canister_entry.c (自動生成)
void canister_init(void) {
    // Pre-allocate 26 pages: 0-9 for canister data, 10-25 for ic-wasm profiling
    ic0_stable64_grow(26);
    ensure_idris2_init();
}
```

WasmBuilder.idrがこれを自動生成する。

### Entry Mode vs Full Tracing

```
Entry Mode (__toggle_entry):
  - エクスポート関数の entry/exit のみ記録
  - 内部関数呼び出しは記録されない
  - メソッドカバレッジ用

Full Tracing (__toggle_tracing):
  - 全関数の entry/exit を記録
  - ※現状空を返す（要調査）
```

## Dead Code Elimination

RefCバックエンドは積極的にデッドコード除去を行う。
`main`から到達不能な関数はWASMに含まれない。

```idris
-- これだけだとcanisterUpdateは除去される
main = canisterInit

-- canisterUpdateを含めるには呼び出しが必要
main = do
  canisterInit
  _ <- canisterUpdate 0
  pure ()
```

## References

- [Emscripten](https://emscripten.org/docs/compiling/WebAssembly.html)
- [Idris2 RefC backend](https://github.com/idris-lang/Idris2)
- [ic-wasm](https://github.com/dfinity/ic-wasm) - WASM instrumentation for ICP

## Upstream Dependencies

### Merged
- **Idris2 RefC WASM比較演算子**: main にマージ済み

### Pending
- **ic-wasm PR**: Merge待ち
