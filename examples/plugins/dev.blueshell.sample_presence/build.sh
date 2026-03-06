#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
PLUGIN_DIR="${ROOT_DIR}/examples/plugins/dev.blueshell.sample_presence"
SRC="${PLUGIN_DIR}/src/sample_presence_plugin.cpp"
OUT_DIR="${PLUGIN_DIR}/bin"

mkdir -p "${OUT_DIR}"

UNAME_S="$(uname -s)"
if [[ "${UNAME_S}" == "Darwin" ]]; then
  OUT_LIB="${OUT_DIR}/libsample_presence.dylib"
  c++ -std=c++20 -dynamiclib -fPIC -I"${ROOT_DIR}/src" "${SRC}" -o "${OUT_LIB}"
elif [[ "${UNAME_S}" == "Linux" ]]; then
  OUT_LIB="${OUT_DIR}/libsample_presence.so"
  c++ -std=c++20 -shared -fPIC -I"${ROOT_DIR}/src" "${SRC}" -o "${OUT_LIB}"
else
  echo "Unsupported platform for this script: ${UNAME_S}"
  echo "Build manually on Windows to bin/sample_presence.dll"
  exit 1
fi

echo "Built sample plugin: ${OUT_LIB}"
