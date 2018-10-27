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
# export EM_CACHE="tmp/emscripten_cache"
export TEMP_DIR="tmp"

# Prepare the cache content so emscripten doesn't try to rebuild it all the time
# cp -r toolchain/emscripten_cache/* tmp/emscripten_cache || true
# Run emscripten to compile and link

IS_TAR=false
if [[ "$@" =~ -o\ ([^[:space:]]*\.js.tar.gz)[[:space:]] ]]; then
  IS_TAR=true
  TAR_FILE=${BASH_REMATCH[1]}
  JS_FILE=${TAR_FILE%.tar.gz}
  ARGS="${@/${TAR_FILE}/${JS_FILE}}"
else
  ARGS="${@}"
fi
python external/emscripten_toolchain/emcc.py ${ARGS}

# Remove the first line of .d file (emscripten resisted all my attempts to make
# it realize it's just the absolute location of the source)
find . -name "*.d" -exec sed -i '2d' {} \;

# Build the tar if we're building a tar
if [[ ${IS_TAR} == "true" ]]; then
  OUT_DIR="$(dirname ${JS_FILE})"
  INPUTS=$(cd "${OUT_DIR}" && find . -type f)
  tar -czf "${TAR_FILE}" -C "${OUT_DIR}" ${INPUTS[@]}
fi
