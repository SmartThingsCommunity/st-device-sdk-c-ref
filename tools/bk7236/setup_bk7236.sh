#!/bin/bash

BSP_NAME=${1}
BSP_PATH=${STDK_REF_PATH}/bsp/${BSP_NAME}
PATCH_PATH=${STDK_REF_PATH}/patches/${BSP_NAME}
CORE_PATH=${STDK_REF_PATH}/iot-core
LIBSODIUM_PATH=${CORE_PATH}/src/deps/libsodium/libsodium

git submodule status bsp/${1} &> /dev/null
if [ "$?" == "0" ]; then
    IS_GIT=1
else
    IS_GIT=0
fi

function apply_patch() {
    TARGET_DIR=${1}
    PATCH_DIR=${2}

    echo "apply-patch : ${TARGET_DIR}"
    pushd ${TARGET_DIR} &> /dev/null
    for patch in ${PATCH_DIR}/*
    do
        if [[ "${patch}" == *".patch" ]]; then
            if [ "${IS_GIT}" == "1" ]; then
                git apply ${patch}
            else
                patch -f -p1 < ${patch}
            fi
        fi
    done
    popd &> /dev/null
}

pushd ${LIBSODIUM_PATH} &> /dev/null
git submodule update --init --recursive
git submodule foreach --recursive git reset --hard

apply_patch ${BSP_PATH} ${PATCH_PATH}

pushd ${BSP_PATH} &> /dev/null
sudo bash tools/env_tools/setup/armino_env_setup.sh
