#!/bin/bash

MAKE_OPTION_ARRAY=("menuconfig" "defconfig" "all" "flash" "clean" "size" "size-components" "size-symbols" "erase_flash" "monitor" "simple_monitor" "list-components" "app" "app-flash" "app-clean" "print_flash_cmd" "help" "bootloader" "bootloader-flash" "bootloader-clean" "partition_table")
OUTPUT_OPTION_ARRAY=("all" "flash" "app" "app-flash" "bootloader" "bootloader-flash" "partition_table")

MAKE_OPTION=all

RELEASE_DATE=`date +%Y%m%d`

CHIP_NAME=${1}
PROJECT_TITLE=${2}
XTENSA_PATH=`dirname ~/esp/xtensa-lx106-elf/bin/.`

export PATH=${XTENSA_PATH}:${PATH}
export IDF_PATH="${PWD}/bsp/${CHIP_NAME}"

STDK_PATH="${PWD}"
CORE_PATH="${PWD}/iot-core"

if [ -e ${STDK_PATH}/.git ]; then
  BUILD_ID_STDK=`git log --pretty=format:'%h' -n 1`
else
  BUILD_ID_STDK=`date +%H%M`
fi

if [ -e ${CORE_PATH}/.git ]; then
  cd ${CORE_PATH}
  BUILD_ID_CORE=`git log --pretty=format:'%h' -n 1`
  cd ${STDK_PATH}
else
  BUILD_ID_CORE=`date +%H%M`
fi

BUILD_ID=${BUILD_ID_STDK}_${BUILD_ID_CORE}

IOT_APPS_PATH="${PWD}/apps/${CHIP_NAME}"
PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"

IOT_OUTPUT_PATH="${PWD}/output/${CHIP_NAME}"


print_usage () {
  echo "    Usage: ./build.sh CHIP_NAME PROJECT_NAME [make_option]"
  echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
  echo "    ex) ./build.sh ${CHIP_NAME} st_switch"
  echo "    ex) ./build.sh ${CHIP_NAME} st_lamp clean"
  echo "    ex) ./build.sh ${CHIP_NAME} st_air_monitor flash"
  echo "    ex) ./build.sh ${CHIP_NAME} st_thermostat monitor"
  echo "    ex) ./build.sh ${CHIP_NAME} st_switch flash monitor"
  echo
}


if [ "${PROJECT_TITLE}" = "" ]; then
  print_usage
  exit
fi
if [ ! -d ${PROJECT_PATH} ]; then
  echo "    Error: Fail to find ${PROJECT_PATH}"
  print_usage
  exit 1
fi

cd ${PROJECT_PATH}
PROJECT_NAME=`cat ./Makefile | grep ^PROJECT_NAME | awk '{print $3}'`

if [ "`cat ./sdkconfig | grep ^CONFIG_PARTITION_TABLE_CUSTOM\= | awk -F'=' '{print $2}'`" = "y" ]; then
  PARTITION_NAME=`cat ./sdkconfig | grep ^CONFIG_PARTITION_TABLE_CUSTOM_FILENAME | awk -F'="' '{print $2}'`
  PARTITION_TABLE=${PROJECT_PATH}
else
  PARTITION_NAME=`cat ./sdkconfig | grep ^CONFIG_PARTITION_TABLE_FILENAME | awk -F'="' '{print $2}'`
  PARTITION_TABLE=${IDF_PATH}/components/partition_table
fi
PARTITION_NAME=${PARTITION_NAME%.*}
PARTITION_TABLE=${PARTITION_TABLE}/${PARTITION_NAME}.csv

if [ ! "${3}" = "" ]; then
  shift 2
  MAKE_OPTION=$@
fi

make ${MAKE_OPTION}

for value in "${OUTPUT_OPTION_ARRAY[@]}"; do
  if [ "${value}" = "${MAKE_OPTION}" ]; then
   OUTPUT_BUILD=y
  fi
done

if [ ! ${OUTPUT_BUILD} ]; then
  exit 0
fi

if [ ! -e ${PROJECT_PATH}/build/bootloader/bootloader.bin ]; then
  echo "Fail to find bootloader.bin"
  exit 1
fi

if [ ! -e ${PROJECT_PATH}/build/${PROJECT_NAME}.bin ]; then
  echo "Fail to find ${PROJECT_NAME}.bin"
  exit 1
fi

if [ ! -e ${PROJECT_PATH}/build/${PARTITION_NAME}.bin ]; then
  echo "Fail to find ${PARTITION_NAME}.bin"
  exit 1
fi
if [ ! -d ${IOT_OUTPUT_PATH} ]; then
  mkdir -p ${IOT_OUTPUT_PATH}
fi

OUTPUT_PATH=${IOT_OUTPUT_PATH}/iotcore_${PROJECT_TITLE}_${RELEASE_DATE}_${BUILD_ID}
if [ ${RELEASE_CONFIG} ]; then
  OUTPUT_PATH=${OUTPUT_PATH}_RELEASE
elif [ ${DEBUG_CONFIG} ]; then
  OUTPUT_PATH=${OUTPUT_PATH}_DEBUG
fi

if [ ! -d ${OUTPUT_PATH} ]; then
  mkdir -p ${OUTPUT_PATH}
else
  rm -rf ${OUTPUT_PATH}/*
fi

cp -f ${PROJECT_PATH}/build/bootloader/bootloader.bin ${OUTPUT_PATH}
cp -f ${PROJECT_PATH}/build/${PROJECT_NAME}.bin ${OUTPUT_PATH}/${PROJECT_NAME}.bin
cp -f ${PROJECT_PATH}/build/${PARTITION_NAME}.bin ${OUTPUT_PATH}/partition.bin

mkdir ${OUTPUT_PATH}/debug
cp -f ${PROJECT_PATH}/build/${PROJECT_NAME}.elf ${OUTPUT_PATH}/debug/
cp -f ${PROJECT_PATH}/build/${PROJECT_NAME}.map ${OUTPUT_PATH}/debug/
cp -f ${PROJECT_PATH}/build/bootloader/bootloader.elf ${OUTPUT_PATH}/debug/
cp -f ${PROJECT_PATH}/build/bootloader/bootloader.map ${OUTPUT_PATH}/debug/
cp -f ${PROJECT_PATH}/sdkconfig ${OUTPUT_PATH}/debug/sdkconfig

BUILD_INFO_FILE=${OUTPUT_PATH}/debug/build_info.txt
date "+%Y/%m/%d %H%M:%S" > ${BUILD_INFO_FILE}
echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}


if [ -d ${IDF_PATH}/.git ]; then
  cat ${IDF_PATH}/.git/config >> ${BUILD_INFO_FILE}
  echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
  git branch -av >> ${BUILD_INFO_FILE}
  echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
  git status >> ${BUILD_INFO_FILE}
  echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
  git diff >> ${BUILD_INFO_FILE}
  echo "---------------------------------------------------------" >> ${BUILD_INFO_FILE}
fi

echo
echo

BOOTLOADER_OFFSET=`cat ${IDF_PATH}/components/bootloader/Makefile.projbuild | grep  -E "BOOTLOADER_OFFSET" | awk -F ':= ' '{print $2}'`
APP_OFFSET=`cat ${PARTITION_TABLE} | grep  -E "factory|ota_0" | awk -F', ' '{print $4}'`
PARTITION_OFFSET=`cat ${PROJECT_PATH}/sdkconfig | grep ^CONFIG_PARTITION_TABLE_OFFSET\= | awk -F'=' '{print $2}'`

ADDRESS_INFO_FILE=${OUTPUT_PATH}/address_info.txt
echo bootloader.bin : ${BOOTLOADER_OFFSET} > ${ADDRESS_INFO_FILE}
echo ${PROJECT_NAME}.bin : ${APP_OFFSET} >> ${ADDRESS_INFO_FILE}
echo partition.bin : ${PARTITION_OFFSET} >> ${ADDRESS_INFO_FILE}

echo "-------------------------------------------------"
echo " binary path: ${OUTPUT_PATH}"
echo "-------------------------------------------------"
