#!/bin/bash
# Build script for Emscripten (run this in the project folder)
# Requires emscripten (emcc) in PATH.
emcc lsm_wasm.cpp -o lsm.js \
  -sEXPORTED_FUNCTIONS=_put,_get,_flush_table,_compact_table \
  -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -sALLOW_MEMORY_GROWTH=1
echo "Build complete. Place lsm.js and lsm.wasm next to lsm2_wasm_connected.html and serve via HTTP."
