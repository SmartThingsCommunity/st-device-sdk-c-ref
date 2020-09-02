#!/bin/bash

export BSP_NAME=${1}
PROJECT="${PWD}/bsp/${BSP_NAME}/project/realtek_ameba1_va0_example"
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"

IOT_APPS_PATH="${PWD}/apps/${BSP_NAME}"
IOT_OUTPUT_PATH="${PWD}/output/${BSP_NAME}"
OUTPUT_BUILD=y
export COMPONENT_PATH="${PWD}/iot-core/src"
export PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"

print_usage () {
	echo "    Usage: python build.py rtl8195 APP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) python build.py rtl8195 switch_example"
	echo "    ex) python build.py rtl8195 light_example"
	echo
}

if [ "${3}" = "clean" ]; then
	OUTPUT_BUILD=n
fi

if [ ! `command -v gawk` ];then
	echo " Enviroment: gawk lost, please install, sudo apt-get install gawk"
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




pushd ${PROJECT}/GCC-RELEASE/ > /dev/null
make clean
#ignore APP_NAME
shift 2

make IMAGE_NAME=dnakit-st $* 2>&1 | tee build.log

popd > /dev/null
if [ ${OUTPUT_BUILD} = y ]; then
	BINARY_PATH=${PROJECT}/GCC-RELEASE/application/Debug/bin
	export OUTPUT_FILE_LIST="${BINARY_PATH}/sdram.bin ${BINARY_PATH}/ram_all.bin ${BINARY_PATH}/ram_2.bin ${BINARY_PATH}/ota.bin"
	export DEBUG_FILE_LIST="${BINARY_PATH}/application.map"
	${STDK_PATH}/tools/common/generate_output.sh
fi
