#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/build-common.sh"

BUILD_TYPE="${1:-Debug}"
case "${BUILD_TYPE}" in
  Debug) BUILD_TYPE_LOWER=debug ;;
  Release) BUILD_TYPE_LOWER=release ;;
  *) echo 'Usage: build.sh [Debug|Release] [CMake options...]' >&2; exit 1 ;;
esac
if (( $# > 0 )); then shift; fi
CMAKE_DIR="cmake-build-${BUILD_TYPE_LOWER}-docker"
CONTAINER_BUILD_DIR="${CONTAINER_SRC_DIR}/${CMAKE_DIR}"
mkdir -p "${DIR}/${CMAKE_DIR}"

# Set this explicitly on every configure so removing default.dol clears a cached patch target.
PATCHED_DOL=""
if [[ -f "${DIR}/default.dol" ]]; then
  PATCHED_DOL="${CONTAINER_BUILD_DIR}/default_mod.dol"
fi
container_run bash -euc '
  cmake -S "$1" -B "$2" -G Ninja -DCMAKE_BUILD_TYPE="$3" -DCREATE_PATCHED_DOL="$4" "${@:5}"
  cmake --build "$2" --config "$3"
' build "${CONTAINER_SRC_DIR}" "${CONTAINER_BUILD_DIR}" "${BUILD_TYPE}" "${PATCHED_DOL}" "$@"
