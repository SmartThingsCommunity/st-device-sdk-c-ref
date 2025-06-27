#!/bin/bash

export BSP_NAME=${1}
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"

IOT_APPS_PATH="apps/${BSP_NAME}"
PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"
BSP_PATH="${STDK_PATH}/bsp/${BSP_NAME}"
BUILD_OUTPUT_PATH="${BSP_PATH}/build/${PROJECT_TITLE}"
FLASH_TOOL_PATH="${STDK_PATH}/tools/${BSP_NAME}"
BIN_FILE="${BUILD_OUTPUT_PATH}/${BSP_NAME}/all-app.bin"
UART_DEVICE="${4}"
UART_PORT="${UART_DEVICE//[!0-9]/}"

# Check device permissions (enhanced)
check_uart_permission() {
    local need_sudo=0

    # Check read/write permissions
    if [ ! -r "${UART_DEVICE}" ] || [ ! -w "${UART_DEVICE}" ]; then
        echo "Error: Missing read/write permissions for [${UART_DEVICE}]" >&2
        echo "Try these solutions:" >&2
        echo "1. Temporary solution (valid until reboot):" >&2
        echo "   sudo chmod 666 ${UART_DEVICE}" >&2
        echo "2. Permanent solution:" >&2
        echo "   sudo usermod -aG dialout ${USER}" >&2
        echo "   Requires re-login to take effect" >&2
        need_sudo=1
    fi

    if [ "${need_sudo}" -eq 1 ]; then
        exit 1
    fi
}

# Verify firmware file
check_bin_file_exist() {
    if [ ! -f "${BIN_FILE}" ]; then
        echo "Error: Firmware file not found [${BIN_FILE}]" >&2
        echo "Please compile the project first" >&2
        exit 1
    fi
}

cd ${BSP_PATH}

#build, clean, erase flash, flash, monitor

if [ "${3}" = "" ]; then
    make "${BSP_NAME}" PROJECT=../../../"${PROJECT_PATH}"
elif [ "${3}" = "clean" ]; then
    rm -rf ${BUILD_OUTPUT_PATH}
elif [ "${3}" = "erase_flash" ]; then
    echo "port: ${UART_PORT}"
    check_uart_permission
    cd "${FLASH_TOOL_PATH}"
    ./bk_loader erase -p "${UART_PORT}" -c
elif [ "${3}" = "flash" ]; then
    check_bin_file_exist
    check_uart_permission
    cd "${FLASH_TOOL_PATH}"
    ./bk_loader download -p "${UART_PORT}" -i "${BIN_FILE}"
elif [ "${3}" = "monitor" ]; then
    check_uart_permission
    python3 "${STDK_PATH}/tools/${BSP_NAME}/monitor.py" "${UART_DEVICE}"
fi
