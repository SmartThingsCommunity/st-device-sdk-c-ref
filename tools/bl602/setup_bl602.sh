#!/bin/bash

install_dependencies() {
    echo "Starting to install necessary tools..."
    sudo apt-get update
    sudo apt-get install -y make gtkterm
    if [ $? -ne 0 ]; then
        echo "Installation failed. Please ensure there are no errors during the installation process."
        exit 1
    else
        echo "Installation completed."
    fi
}

install_dependencies

BSP_NAME=${1}
BSP_PATH=${STDK_REF_PATH}/bsp/${BSP_NAME}
PATCH_PATH=${STDK_REF_PATH}/patches/${BSP_NAME}
CORE_PATH=${STDK_REF_PATH}/iot-core
LIBSODIUM_PATH=${CORE_PATH}/src/deps/libsodium/libsodium
JSON_PATH=${CORE_PATH}/src/deps/json/cJSON

git submodule status bsp/${BSP_NAME} &> /dev/null
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
 
pushd {LIBSODIUM_PATH} &> /dev/null
git submodule update --init --recursive
git submodule foreach --recursive git reset --hard

pushd {LIBSODIUM_PATH} &> /dev/null
git submodule update --init --recursive
git submodule foreach --recursive git reset --hard

pushd ${BSP_PATH} &> /dev/null
git submodule update --init --recursive
git submodule foreach --recursive git reset --hard

apply_patch ${BSP_PATH} ${PATCH_PATH}
