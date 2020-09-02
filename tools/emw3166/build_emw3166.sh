#!/bin/bash
export BOARD_NAME=MK3166
export CHIP_NAME=${1}
PROJECT="${PWD}/bsp/${CHIP_NAME}/"
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"

export IOT_APPS_PATH="${PWD}/apps/${CHIP_NAME}"
IOT_OUTPUT_PATH="${PWD}/output/${CHIP_NAME}"
OUTPUT_BUILD=y
export COMPONENT_PATH="${PWD}/iot-core/src"
export PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"
print_usage () {
	echo "    Usage: python build.py emw3166 APP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) python build.py emw3166 switch_example"
	echo "    ex) python build.py emw3166 light_example"
	echo
}

if [ "${3}" = "clean" ]; then
	pushd ${PWD}/bsp/${CHIP_NAME} > /dev/null
	if [ -d "build" ]; then
		rm -rf build
	fi
	popd > /dev/null
	exit
fi

if [ ! `command -v gawk` ];then
	echo " Environment: gawk lost, please install, sudo apt-get install gawk"
	exit
fi

if [ "${PROJECT_TITLE}" = "" ]; then
	print_usage
	exit
fi
if [ ! -d ${PROJECT_PATH} ]; then
	echo "    Error: Fail to find ${PROJECT_PATH}"
	exit 1
fi

pushd ${PWD}/bsp/${CHIP_NAME}/ > /dev/null
mico make ${2}@${BOARD_NAME} ${3}
popd > /dev/null
if [ ${OUTPUT_BUILD} = y ]; then
	BINARY_PATH=${PWD}/bsp/${CHIP_NAME}/build/${2}@${BOARD_NAME}/binary
	export OUTPUT_FILE_LIST="${BINARY_PATH}/${2}@${BOARD_NAME}.bin"
	export DEBUG_FILE_LIST="${BINARY_PATH}/${2}@${BOARD_NAME}.map"
	${STDK_PATH}/tools/common/generate_output.sh
fi
