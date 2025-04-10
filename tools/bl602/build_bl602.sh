#!/bin/sh

export CHIP_NAME="BL602"
export BSP_NAME=${1}
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"


IOT_APPS_PATH="${PWD}/apps/${BSP_NAME}"
PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"
BUILD_OUTPUT_PATH="${PROJECT_PATH}/build_out"
BSP_PATH="${STDK_PATH}/bsp/${BSP_NAME}"
BL_FLASH_TOOL="${BSP_PATH}/tools/flash_tool/bflb_iot_tool-ubuntu"
BAUDRATE="2000000"
UART_PORT="/dev/ttyACM0"
PARTITION_TABLE="${BSP_PATH}/tools/flash_tool/chips/bl602/partition/partition_cfg_2M.toml"
APP_FIRMWARE_PATH="${BUILD_OUTPUT_PATH}/${PROJECT_TITLE}.bin"


cd ${PROJECT_PATH}

# build, clean, erase flash, flash, monitor

if [ "${3}" = "" ]; then
    make CONFIG_CHIP_NAME=BL602 CONFIG_LINK_ROM=1 CONFIG_BLE_TP_SERVER=1 CONFIG_BLECONTROLLER_LIB=all -j PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC=1 CONFIG_BT_STACK_PTS=1
elif [ "${3}" = "clean" ]; then
    rm -rf ${BUILD_OUTPUT_PATH}
elif [ "${3}" = "erase_flash" ]; then
    "${BL_FLASH_TOOL}" --chipname="${CHIP_NAME}" --baudrate="${BAUDRATE}" --port="${4}" --pt="${PARTITION_TABLE}" --dts=  --firmware="${APP_FIRMWARE_PATH}" --erase
elif [ "${3}" = "flash" ]; then
    if [ ! -d "${BUILD_OUTPUT_PATH}" ]; then
        make CONFIG_CHIP_NAME=BL602 CONFIG_LINK_ROM=1 CONFIG_BLE_TP_SERVER=1 CONFIG_BLECONTROLLER_LIB=all -j PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC=1 CONFIG_BT_STACK_PTS=1
    fi
    "${BL_FLASH_TOOL}" --chipname="${CHIP_NAME}" --baudrate="${BAUDRATE}" --port="${4}" --pt="${PARTITION_TABLE}" --dts=  --firmware="${APP_FIRMWARE_PATH}"
elif [ "${3}" = "monitor" ]; then
    /usr/bin/python3 "${STDK_PATH}/tools/${BSP_NAME}/monitor.py" "${4}"
fi


