#!/bin/bash

CHIP_LIST=("esp8266" "esp32" "rtl8710" "rtl8195")
CORE_PATH="iot-core"
CHIP_NAME=${1}
print_usage () {
	echo "    Usage: ./setup.sh CHIP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) ./setup.sh esp8266"
	echo "    ex) ./setup.sh esp32"
	echo "    ex) ./setup.sh rtl8710"
	echo "    ex) ./setup.sh rtl8195"
	echo
}

if [ "${CHIP_NAME}" == "" ]; then
	print_usage
	exit 0
fi

if [ ! -e tools/${CHIP_NAME}/setup_${CHIP_NAME}.sh ]; then
	echo "Failed to find tools/${CHIP_NAME}/setup_${CHIP_NAME}.sh"
	print_usage
	exit 1
fi

git submodule status ${CORE_PATH} &> /dev/null
if [ "$?" == "0" ]; then
	git submodule sync ${CORE_PATH}
	git submodule init ${CORE_PATH}
	git submodule update ${CORE_PATH}
	cd ${CORE_PATH}
	git reset --hard HEAD
	cd ..
fi

git submodule status bsp/${CHIP_NAME} &> /dev/null
if [ "$?" == "0" ]; then
	git submodule sync bsp/${CHIP_NAME}
	git submodule init bsp/${CHIP_NAME}
	git submodule update bsp/${CHIP_NAME}
	cd bsp/${CHIP_NAME}
	git reset --hard HEAD
	cd ../..
fi

tools/${CHIP_NAME}/setup_${CHIP_NAME}.sh $@
