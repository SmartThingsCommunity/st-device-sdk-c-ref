#!/bin/bash

export BSP_NAME=${1}
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"

IOT_APPS_PATH="${PWD}/apps/${BSP_NAME}"
PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"

XTENSA_PATH=`dirname ~/esp/xtensa-lx106-elf/bin/.`
export PATH=${XTENSA_PATH}:${PATH}
export IDF_PATH="${PWD}/bsp/${BSP_NAME}"

MAKE_OPTION_ARRAY=("menuconfig" "defconfig" "all" "flash" "clean" "size" "size-components" "size-symbols" "erase_flash" "monitor" "simple_monitor" "list-components" "app" "app-flash" "app-clean" "print_flash_cmd" "help" "bootloader" "bootloader-flash" "bootloader-clean" "partition_table")
OUTPUT_OPTION_ARRAY=("all" "flash" "app" "app-flash" "bootloader" "bootloader-flash" "partition_table")

MAKE_OPTION=all

print_usage () {
  echo "    Usage: python build.py BSP_NAME PROJECT_NAME [make_option]"
  echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
  echo "    ex) python build.py ${BSP_NAME} st_switch"
  echo "    ex) python build.py ${BSP_NAME} st_lamp clean"
  echo "    ex) python build.py ${BSP_NAME} st_air_monitor flash"
  echo "    ex) python build.py ${BSP_NAME} st_thermostat monitor"
  echo "    ex) python build.py ${BSP_NAME} st_switch flash monitor"
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

### Build

cd ${PROJECT_PATH}



if [ ! "${3}" = "" ]; then
  shift 2
  MAKE_OPTION=$@
fi

make ${MAKE_OPTION}

if [ ! "${?}" = "0" ]; then
  exit ${?}
fi

for value in "${OUTPUT_OPTION_ARRAY[@]}"; do
  if [[ "${MAKE_OPTION}" == *"${value}"* ]]; then
   OUTPUT_BUILD=y
  fi
done

if [ ! ${OUTPUT_BUILD} ]; then
  exit 0
fi

### Write address_info.txt
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

PARTITION_TABLE_OFFSET=`cat ${PROJECT_PATH}/sdkconfig | grep  -E "CONFIG_PARTITION_TABLE_OFFSET" | awk -F '=' '{print $2}'`
GET_PART_INFO="${STDK_PATH}/bsp/${BSP_NAME}/components/partition_table/parttool.py -q --partition-table-file ${PROJECT_PATH}/build/${PARTITION_NAME}.bin --partition-table-offset ${PARTITION_TABLE_OFFSET}"


BOOTLOADER_OFFSET=`cat ${IDF_PATH}/components/bootloader/Makefile.projbuild | grep  -E "BOOTLOADER_OFFSET" | awk -F ':= ' '{print $2}'`
APP_OFFSET=`${GET_PART_INFO} get_partition_info --partition-boot-default --info offset`
OTA_DATA_OFFSET=`${GET_PART_INFO} get_partition_info --partition-type data --partition-subtype ota --info offset`
PARTITION_OFFSET=`cat ${PROJECT_PATH}/sdkconfig | grep ^CONFIG_PARTITION_TABLE_OFFSET\= | awk -F'=' '{print $2}'`

ADDRESS_INFO_FILE=${PROJECT_PATH}/address_info.txt
echo ota_data_initial.bin : ${OTA_DATA_OFFSET} > ${ADDRESS_INFO_FILE}
echo bootloader.bin : ${BOOTLOADER_OFFSET} >> ${ADDRESS_INFO_FILE}
echo ${PROJECT_NAME}.bin : ${APP_OFFSET} >> ${ADDRESS_INFO_FILE}
echo partition.bin : ${PARTITION_OFFSET} >> ${ADDRESS_INFO_FILE}

### Generate output
export OUTPUT_FILE_LIST="${PROJECT_PATH}/build/ota_data_initial.bin ${PROJECT_PATH}/build/bootloader/bootloader.bin ${PROJECT_PATH}/build/${PROJECT_NAME}.bin ${PROJECT_PATH}/build/${PARTITION_NAME}.bin ${ADDRESS_INFO_FILE}"

export DEBUG_FILE_LIST="${PROJECT_PATH}/build/${PROJECT_NAME}.elf ${PROJECT_PATH}/build/${PROJECT_NAME}.map ${PROJECT_PATH}/build/bootloader/bootloader.elf ${PROJECT_PATH}/build/bootloader/bootloader.map ${PROJECT_PATH}/sdkconfig"

${STDK_PATH}/tools/common/generate_output.sh
