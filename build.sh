#!/bin/bash

CHIP_LIST=("esp8266" "esp32" "rtl8710" "rtl8195")

CHIP_NAME=${1}
APP_NAME=${2}

export SS_VER=`cat ./VERSION`-`git describe --always --tags --dirty`

print_usage () {
	echo "    Usage: ./build.sh CHIP_NAME APP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) ./build.sh esp8266 smart_switch"
	echo "    ex) ./build.sh esp32 smart_lamp"
	echo "    ex) ./build.sh rtl8710 smart_switch"
	echo "    ex) ./build.sh rtl8710 st_switch"
	echo "    ex) ./build.sh rtl8195 smart_switch"
	echo "    ex) ./build.sh rtl8195 st_switch"
	echo
}

if [ "${CHIP_NAME}" == "" ]; then
	print_usage
	exit 0
fi

if [ ! -e tools/${CHIP_NAME}/build_${CHIP_NAME}.sh ]; then
	echo "Failed to find tools/${CHIP_NAME}/build_${CHIP_NAME}.sh"
	print_usage
	exit 1
fi

if [ ! -d apps/${CHIP_NAME}/${APP_NAME} ]; then
	echo "Failed to find apps/${CHIP_NAME}/${APP_NAME}"
	print_usage
	exit 1
fi

tools/${CHIP_NAME}/build_${CHIP_NAME}.sh $@
