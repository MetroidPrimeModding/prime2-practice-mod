#!/usr/bin/env bash
# Shared by local build/patch helpers; CI invokes build.sh and uses this same pin.
IMAGE="ghcr.io/metroidprimemodding/gcn-static-patcher/build@sha256:ed4997cbed8a782bb88d92e8c08fe84e4cc4d30f86d4b02a5abe52076feaee26"
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONTAINER_SRC_DIR=/tmp/prime2-practice-mod

if command -v podman >/dev/null 2>&1; then
  CONTAINER_RUNTIME=podman
elif command -v docker >/dev/null 2>&1; then
  CONTAINER_RUNTIME=docker
else
  echo 'Install Podman or Docker to build Echoes Practice Mod.' >&2
  exit 1
fi

container_run() {
  "${CONTAINER_RUNTIME}" run --rm \
    -v "${DIR}:${CONTAINER_SRC_DIR}:z" \
    "${IMAGE}" "$@"
}
