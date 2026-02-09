#!/bin/bash -xe

test -f default.dol || (echo "default.dol not found" && exit 1)

CREATE_PATCHED_DOL=default_mod.dol

# call build with all params; this will set some env vars we use later
source ./build.sh "$@"

# copy the built binary to the patcher dir
cp -v "${EXTERNAL_BUILD_DIR}/default_mod.dol" ../mp2/files/default.dol
cp -v "${EXTERNAL_BUILD_DIR}/default_mod.dol" ../mp2/sys/main.dol

