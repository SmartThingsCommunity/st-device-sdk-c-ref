#!/bin/bash

generate_output() {

  OUTPUT_FILE_ARRAY=($OUTPUT_FILE_LIST)
  DEBUG_FILE_ARRAY=($DEBUG_FILE_LIST)
  for value in "${OUTPUT_FILE_ARRAY[@]}"; do
    if [ ! -e ${value} ]; then
      echo "Fail to find ${value}"
      exit 1
    fi  
  done

  RELEASE_DATE=`date +%Y%m%d`

  if [ -e ${STDK_PATH}/.git ]; then
    pushd ${STDK_PATH} > /dev/null
    BUILD_ID_STDK=`git log --pretty=format:'%h' -n 1`
    popd > /dev/null
  else
    BUILD_ID_STDK=`date +%H%M`
  fi

  if [ -e ${CORE_PATH}/.git ]; then
    pushd ${CORE_PATH} > /dev/null
    BUILD_ID_CORE=`git log --pretty=format:'%h' -n 1`
    popd > /dev/null
  else
    BUILD_ID_CORE=`date +%H%M`
  fi

  BUILD_ID=${BUILD_ID_STDK}_${BUILD_ID_CORE}

  OUTPUT_DIR_NAME=iotcore_${PROJECT_TITLE}_${RELEASE_DATE}_${BUILD_ID}
  if [ ${RELEASE_CONFIG} ]; then
    OUTPUT_DIR_NAME=${OUTPUT_DIR_NAME}_RELEASE
  elif [ ${DEBUG_CONFIG} ]; then
    OUTPUT_DIR_NAME=${OUTPUT_DIR_NAME}_DEBUG
  fi

  OUTPUT_PATH=${STDK_PATH}/output/${BSP_NAME}/${OUTPUT_DIR_NAME}

  if [ ! -d ${OUTPUT_PATH} ]; then
    mkdir -p ${OUTPUT_PATH}
  else
    rm -rf ${OUTPUT_PATH}/*
  fi

  for value in "${OUTPUT_FILE_ARRAY[@]}"; do
    cp -f ${value} ${OUTPUT_PATH}/
  done

  mkdir ${OUTPUT_PATH}/debug
  for value in "${DEBUG_FILE_ARRAY[@]}"; do
    cp -f ${value} ${OUTPUT_PATH}/debug/
  done

  BUILD_INFO_FILE=${OUTPUT_PATH}/debug/build_info.txt
  date "+%Y/%m/%d %H:%M:%S" > ${BUILD_INFO_FILE}

  if [ -d ${STDK_PATH}/.git ]; then
    pushd ${STDK_PATH} > /dev/null
    echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
    cat ${STDK_PATH}/.git/config >> ${BUILD_INFO_FILE}
    echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
    git branch -av >> ${BUILD_INFO_FILE}
    echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
    git status >> ${BUILD_INFO_FILE}
    echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
    git diff >> ${BUILD_INFO_FILE}
    echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
    git submodule foreach git diff >> ${BUILD_INFO_FILE}
    popd > /dev/null
  fi

  pushd ${STDK_PATH}/output/${BSP_NAME}/
  ln -Tfs ${OUTPUT_DIR_NAME} iotcore_${PROJECT_TITLE}_latest
  popd

  echo
  echo "-------------------------------------------------"
  echo "binary path: ${OUTPUT_PATH}"
  echo "-------------------------------------------------"
}

generate_output
