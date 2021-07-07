#!/bin/bash

export BSP_NAME=${1}
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"

IOT_APPS_PATH="${PWD}/apps/${BSP_NAME}"
PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"
COMMON_TOOLS_PATH="${STDK_PATH}/tools/common"

export IDF_PATH="${PWD}/bsp/${BSP_NAME}"

MAKE_OPTION_ARRAY=("menuconfig" "defconfig" "all" "flash" "clean" "size" "size-components" "size-symbols" "erase_flash" "monitor" "simple_monitor" "list-components" "app" "app-flash" "app-clean" "print_flash_cmd" "help" "bootloader" "bootloader-flash" "bootloader-clean" "partition_table")
OUTPUT_OPTION_ARRAY=("build" "all" "flash" "app" "app-flash" "bootloader" "bootloader-flash" "partition_table")

MAKE_OPTION=build

print_usage () {
  echo "    Usage: python build.py BSP_NAME PROJECT_NAME [make_option]"
  echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
  echo "    ex) python build.py ${BSP_NAME} st_switch"
  echo "    ex) python build.py ${BSP_NAME} st_lamp clean"
  echo "    ex) python build.py ${BSP_NAME} st_switch flash"
  echo "    ex) python build.py ${BSP_NAME} st_lamp monitor"
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

cp ${CORE_PATH}/src/Kconfig ${IDF_PATH}/components/iot-core_config/Kconfig

### Build

cd ${PROJECT_PATH}

if [ ! "${3}" = "" ]; then
  shift 2
  MAKE_OPTION=$@
fi

pushd ${IDF_PATH}
. ./export.sh
popd
idf.py ${MAKE_OPTION}

if [ ! "${?}" = "0" ]; then
  exit ${?}
fi

for value in "${OUTPUT_OPTION_ARRAY[@]}"; do
  if [ "${value}" = "${MAKE_OPTION}" ]; then
   OUTPUT_BUILD=y
  fi
done

if [ ! ${OUTPUT_BUILD} ]; then
  exit 0
fi

### Write address_info.txt
PROJECT_NAME=`cat ./Makefile | grep ^PROJECT_NAME | awk '{print $3}'`

### Generate output
export OUTPUT_FILE_LIST="${PROJECT_PATH}/build/ota_data_initial.bin ${PROJECT_PATH}/build/bootloader/bootloader.bin ${PROJECT_PATH}/build/${PROJECT_NAME}.bin ${PROJECT_PATH}/build/partition_table/partition-table.bin"

export DEBUG_FILE_LIST="${PROJECT_PATH}/build/${PROJECT_NAME}.elf ${PROJECT_PATH}/build/${PROJECT_NAME}.map ${PROJECT_PATH}/build/bootloader/bootloader.elf ${PROJECT_PATH}/build/bootloader/bootloader.map ${PROJECT_PATH}/sdkconfig"

${STDK_PATH}/tools/common/generate_output.sh

python ${COMMON_TOOLS_PATH}/check_submodule.py ${IDF_PATH}

