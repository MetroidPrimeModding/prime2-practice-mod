#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/build-common.sh"
BUILD_TYPE="${1:-Debug}"
case "${BUILD_TYPE}" in
  Debug) BUILD_TYPE_LOWER=debug ;;
  Release) BUILD_TYPE_LOWER=release ;;
  *) echo 'Usage: build_and_copy.sh [Debug|Release] [CMake options...]' >&2; exit 1 ;;
esac
if (( $# > 0 )); then shift; fi
[[ -f "${DIR}/default.dol" ]] || { echo 'default.dol not found in repository root' >&2; exit 1; }
[[ -d "${DIR}/../mp2/files" && -d "${DIR}/../mp2/sys" ]] || {
  echo 'Expected ../mp2/files and ../mp2/sys destination directories' >&2; exit 1;
}
# Always produce the artifact we install, even if the caller supplied an ELF-only option.
"${DIR}/build.sh" "${BUILD_TYPE}" "$@" \
  "-DCREATE_PATCHED_DOL=${CONTAINER_SRC_DIR}/cmake-build-${BUILD_TYPE_LOWER}-docker/default_mod.dol"
cp -v "${DIR}/cmake-build-${BUILD_TYPE_LOWER}-docker/default_mod.dol" "${DIR}/../mp2/files/default.dol"
cp -v "${DIR}/cmake-build-${BUILD_TYPE_LOWER}-docker/default_mod.dol" "${DIR}/../mp2/sys/main.dol"
