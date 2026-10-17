#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/build-common.sh"
BUILD_TYPE="${1:-Debug}"
case "${BUILD_TYPE}" in
  Debug) BUILD_TYPE_LOWER=debug ;;
  Release) BUILD_TYPE_LOWER=release ;;
  *) echo 'Usage: build_and_patch_iso.sh [Debug|Release] [CMake options...]' >&2; exit 1 ;;
esac
[[ -f "${DIR}/prime2.iso" ]] || { echo 'prime2.iso not found in repository root' >&2; exit 1; }
"${DIR}/build.sh" "$@"
container_run gcn-static-patcher-cli \
  -m "${CONTAINER_SRC_DIR}/cmake-build-${BUILD_TYPE_LOWER}-docker/prime2-practice" \
  -i "${CONTAINER_SRC_DIR}/prime2.iso" \
  -o "${CONTAINER_SRC_DIR}/prime2-practice-mod.iso" --overwrite
