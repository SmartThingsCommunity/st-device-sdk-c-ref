#!/bin/bash

CHIP_LIST=("esp8266" "esp32" "rtl8710" "rtl8195")

CHIP_NAME=${1}
APP_NAME=${2}

print_usage () {
	echo "    Usage: ./build.sh CHIP_NAME APP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) ./build.sh esp8266 st_switch"
	echo "    ex) ./build.sh esp32 st_lamp"
	echo "    ex) ./build.sh rtl8710 st_switch"
	echo "    ex) ./build.sh rtl8195 st_lamp"
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

tools/${CHIP_NAME}/build_${CHIP_NAME}.sh $@
