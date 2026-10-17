#!/usr/bin/env bash
# Compatibility entry point: builds and packages locally; never publishes.
set -euo pipefail
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if (( $# > 1 )); then echo 'Usage: build_release.sh [version]' >&2; exit 1; fi
VERSION="$("${DIR}/release_version.sh")"
if [[ "${1:-${VERSION}}" != "${VERSION}" ]]; then
  echo 'Requested version must match src/patcher_config.toml' >&2
  exit 1
fi
"${DIR}/release_notes.sh" "${VERSION}" >/dev/null
[[ ! -e "${DIR}/release/${VERSION}" ]] || { echo "release/${VERSION} already exists" >&2; exit 1; }
"${DIR}/build.sh" Release -DRANDOMIZER=OFF -DCREATE_PATCHED_DOL=
exec "${DIR}/package_release.sh" "${VERSION}" "${DIR}/cmake-build-release-docker/prime2-practice" "${DIR}/release/${VERSION}"
