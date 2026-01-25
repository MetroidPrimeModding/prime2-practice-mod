#!/bin/bash -xe

# call build with all params; this will set some env vars we use later
source ./build.sh "$@"

docker run --rm -v "${SCRIPT_DIR}/..":"${DOCKER_SRC_DIR}" "${IMAGE}" bash -c "\
  gcn-static-patcher-cli \
    -m \"${DOCKER_SRC_DIR}/cmake-build-release-docker/prime2-practice\" \
    -i \"${DOCKER_SRC_DIR}/prime2.iso\" \
    -o \"${DOCKER_SRC_DIR}/prime2-practice-mod.iso\" \
    --overwrite"
