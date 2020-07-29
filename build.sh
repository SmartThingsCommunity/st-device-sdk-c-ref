#!/bin/bash

export STDK_CORE_PATH=`readlink -e -n "./iot-core"`

BSP_NAME=${1}
APP_NAME=${2}
ARGUMENTS=$@

if [[ "${1}" == "apps/"*"/"?* ]]; then
    BSP_NAME=`echo ${1} | awk -F'/' '{print $2}'`
    APP_NAME=`echo ${1} | awk -F'/' '{print $3}'`
    shift 1
    ARGUMENTS="${BSP_NAME} ${APP_NAME} $@"
fi

if [ ${BSP_NAME} == "esp32" ]; then
    BSP_NAME="esp32_v3.3"
    ARGUMENTS=`echo ${ARGUMENTS} | sed 's/esp32/esp32_v3.3/'`
fi

print_usage () {
    echo "    Usage: ./build.sh BSP_NAME APP_NAME"
    echo "                     or "
    echo "           ./build.sh apps/BSP_NAME/APP_NAME"
    echo "- - - - - - - - - - - - - - - - - - -"
    echo "    ex) ./build.sh apps/esp8266/st_switch"
    echo "    ex) ./build.sh apps/esp32/st_switch"
    echo "    ex) ./build.sh rtl8195 st_switch"
    echo "    ex) ./build.sh rtl8720c st_switch"
    echo "    ex) ./build.sh rtl8721c st_switch"
    echo "    ex) ./build.sh emw3166 st_switch"
    echo
}

print_path() {
    if [ "${BSP_NAME}" == "" -o ! -d apps/${BSP_NAME} ]; then
        echo "Fail to find BSP_NAME (apps/${BSP_NAME}) "
        echo "ls apps/"
        ls apps/
    elif [ "${APP_NAME}" == "" -o ! -d apps/${BSP_NAME}/${APP_NAME} ]; then
        echo "Fail to find APP_NAME (apps/${BSP_NAME}/${APP_NAME})"
        echo "ls apps/${BSP_NAME}/"
        ls apps/${BSP_NAME}
    fi
    echo
}

if [ "${BSP_NAME}" == "" ]; then
    print_usage
    print_path
    exit 0
fi

if [ ! -e tools/${BSP_NAME}/build_${BSP_NAME}.sh ]; then
    echo "Failed to find tools/${BSP_NAME}/build_${BSP_NAME}.sh"
    print_usage
    print_path
    exit 1
fi

tools/${BSP_NAME}/build_${BSP_NAME}.sh ${ARGUMENTS}
if [ "$?" -ne "0" ]; then
    print_path
fi
