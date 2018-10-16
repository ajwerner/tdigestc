#!/bin/bash    
# toolchain/emcc.sh
set -euo pipefail

export LLVM_ROOT='external/emscripten_clang'
export EMSCRIPTEN_NATIVE_OPTIMIZER='external/emscripten_clang/optimizer'
export BINARYEN_ROOT='external/emscripten_clang/'
export NODE_JS=''
export EMSCRIPTEN_ROOT='external/emscripten_toolchain'
export SPIDERMONKEY_ENGINE=''
export EM_EXCLUSIVE_CACHE_ACCESS=1
export EMCC_SKIP_SANITY_CHECK=1
export EMCC_WASM_BACKEND=0

mkdir -p "tmp/emscripten_cache"
export EM_CACHE="tmp/emscripten_cache"
export TEMP_DIR="tmp"

# Prepare the cache content so emscripten doesn't try to rebuild it all the time
cp -r toolchain/emscripten_cache/* tmp/emscripten_cache
# Run emscripten to compile and link
set -x

python external/emscripten_toolchain/emar.py "$@" 
