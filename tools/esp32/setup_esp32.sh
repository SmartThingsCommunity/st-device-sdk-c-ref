#!/bin/bash

BSP_NAME=${1}
BSP_PATH=${STDK_REF_PATH}/bsp/${BSP_NAME}
PATCH_PATH=${STDK_REF_PATH}/patches/${BSP_NAME}

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
        git am ${patch}
      else
        patch -f -p1 < ${patch}
      fi
    fi
  done
  popd &> /dev/null
}

pushd ${BSP_PATH} &> /dev/null
git submodule update --init --recursive
git submodule foreach --recursive git reset --hard

./install.sh

if [ "$?" == "0" ]; then
  echo "(run in ${BSP_PATH},"
  echo " OR use SmartThings Device SDK build script)"
fi

apply_patch ${BSP_PATH} ${PATCH_PATH}

pushd ${BSP_PATH} &> /dev/null
rm -rf components/iot-core_config
mkdir components/iot-core_config
echo "idf_component_register()" > components/iot-core_config/CMakeLists.txt
cp ${STDK_CORE_PATH}/src/Kconfig ${BSP_PATH}/components/iot-core_config/Kconfig
