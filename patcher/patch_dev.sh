#!/usr/bin/env bash -xe
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "${SCRIPT_DIR}/.."
./build_and_copy.sh

cd "${SCRIPT_DIR}"
PYTHONPATH="${SCRIPT_DIR}" python3 main.py iso --skip-hash -i prime2.iso -o metroid-prime2-practice-mod.iso -m ../cmake-build-debug-docker/prime-practice
