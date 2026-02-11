#!/bin/bash -xe
# param 1: build type (Debug/Release) - default Debug
BUILD_TYPE="${1:-Debug}"
# make build dir names lowercase (this is more cross-platform)
BUILD_TYPE_LOWER="$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "${DIR}"

# -DCREATE_PATCHED_DOL= default_mod.dol

# these are exported so build_and_copy.sh can use them too
IMAGE="ghcr.io/metroidprimemodding/gcn-static-patcher/build:latest"
CMAKE_DIR="cmake-build-${BUILD_TYPE_LOWER}-docker" # same as my clion for convenience
EXTERNAL_SRC_DIR="./"
EXTERNAL_BUILD_DIR="${EXTERNAL_SRC_DIR}${CMAKE_DIR}"
DOCKER_SRC_DIR="/tmp/prime2-practice-mod/"
DOCKER_BUILD_DIR="${DOCKER_SRC_DIR}${CMAKE_DIR}"

if [ "${CREATE_PATCHED_DOL}" != "" ]; then
    EXTRA_CMAKE_ARGUMENTS="-DCREATE_PATCHED_DOL=${DOCKER_BUILD_DIR}/${CREATE_PATCHED_DOL}"
fi

mkdir -p "${EXTERNAL_BUILD_DIR}"

# launch a build in a docker container first (this does the same thing intellij would do)
docker run --rm \
    -w "${DOCKER_BUILD_DIR}" \
    -v "${EXTERNAL_SRC_DIR}":"${DOCKER_SRC_DIR}" \
    "${IMAGE}" \
    bash -c "cmake "${DOCKER_SRC_DIR}" -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ${EXTRA_CMAKE_ARGUMENTS} -G Ninja && cmake --build . --config ${BUILD_TYPE}"
