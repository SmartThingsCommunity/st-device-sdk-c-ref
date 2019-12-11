#!/bin/bash

CHIP_NAME=${1}
PROJECT="${PWD}/bsp/${CHIP_NAME}/project/realtek_amebaz2_v0_example"
PROJECT_TITLE=${2}
STDK_PATH="${PWD}"
IOT_APPS_PATH="${PWD}/apps/${CHIP_NAME}"
IOT_OUTPUT_PATH="${PWD}/output/${CHIP_NAME}"
OUTPUT_BUILD=y
export COMPONENT_PATH="${PWD}/iot-core/src"
export PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"

print_usage () {
	echo "    Usage: ./build.sh rtl8720c APP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) ./build.sh rtl8720c smart_switch"
	echo "    ex) ./build.sh rtl8720c st_switch"
	echo
}

if [ "${3}" = "clean" ]; then
	OUTPUT_BUILD=n
fi

generate_output() {

	RELEASE_DATE=`date +%Y%m%d`

	if [ -e ${STDK_PATH}/.git ]; then
		pushd ${COMPONENT_PATH} > /dev/null
		BUILD_ID_STDK=`git log --pretty=format:'%h' -n 1`
		popd > /dev/null
	else
		BUILD_ID_STDK=`date +%H%M`
	fi

	if [ -e ${COMPONENT_PATH}/.git ]; then
		pushd ${COMPONENT_PATH} > /dev/null
		BUILD_ID_CORE=`git log --pretty=format:'%h' -n 1`
		popd > /dev/null
	else
		BUILD_ID_CORE=`date +%H%M`
	fi

	BUILD_ID=${BUILD_ID_STDK}_${BUILD_ID_CORE}

	OUTPUT_PATH=${IOT_OUTPUT_PATH}/iotcore_${PROJECT_TITLE}_${RELEASE_DATE}_${BUILD_ID}
	if [ ${RELEASE_CONFIG} ]; then
		OUTPUT_PATH=${OUTPUT_PATH}_RELEASE
	elif [ ${DEBUG_CONFIG} ]; then
		OUTPUT_PATH=${OUTPUT_PATH}_DEBUG
	fi

	if [ ! -d ${OUTPUT_PATH} ]; then
		mkdir -p ${OUTPUT_PATH}
	else
		rm -rf ${OUTPUT_PATH}/*
	fi

	BINARY_PATH=${PROJECT}/GCC-RELEASE/application_is/Debug/bin
	cp -f ${BINARY_PATH}/flash_is.bin ${OUTPUT_PATH}/
	cp -f ${BINARY_PATH}/firmware_is.bin ${OUTPUT_PATH}/

	mkdir ${OUTPUT_PATH}/debug
	cp -f ${BINARY_PATH}/application_is.map ${OUTPUT_PATH}/debug/
	echo "-------------------------------------------------"
	echo "binary path: ${OUTPUT_PATH}"
	echo "-------------------------------------------------"
}

if [ "${PROJECT_TITLE}" = "" ]; then
	print_usage
	exit
fi
if [ ! -d ${PROJECT_PATH} ]; then
	echo "    Error: Fail to find ${PROJECT_PATH}"
	exit 1
fi




pushd ${PROJECT}/GCC-RELEASE/ > /dev/null
# make clean
#ignore APP_NAME
shift 2

make IMAGE_NAME=dnakit-st $* 2>&1 | tee build.log

if [ -e application/Debug/bin/application.map ] && [ -e application/Debug/bin/application.nmap ]; then
    # show remaining heap size
    msp_ram=$(grep MSP_RAM application/Debug/bin/application.map | awk '{ print $2 }')
    end=$(grep " end$" application/Debug/bin/application.nmap | awk '{ printf $1}')
    echo -n "Remaining heap is "
    awk "BEGIN { print $msp_ram - 0x$end }"
fi

popd > /dev/null
if [ ${OUTPUT_BUILD} = y ]; then
	generate_output
fi
