#!/bin/bash

export BSP_NAME=${1}
PROJECT="${PWD}/bsp/${BSP_NAME}/project/realtek_amebaD_va0_example"
export PROJECT_TITLE=${2}

export STDK_PATH="${PWD}"
export CORE_PATH="${PWD}/iot-core"

IOT_APPS_PATH="${PWD}/apps/${BSP_NAME}"
IOT_OUTPUT_PATH="${PWD}/output/${BSP_NAME}"
OUTPUT_BUILD=y
export COMPONENT_PATH="${PWD}/iot-core/src"
export PROJECT_PATH="${IOT_APPS_PATH}/${PROJECT_TITLE}"

print_usage () {
	echo "    Usage: python build.py rtl8721c APP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) python build.py rtl8721c switch_example"
	echo "    ex) python build.py rtl8721c light_example"
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

pushd ${PROJECT}/GCC-RELEASE/project_lp > /dev/null
shift 2

make $* 2>&1 | tee build.log
popd > /dev/null

pushd ${PROJECT}/GCC-RELEASE/project_hp > /dev/null
shift 2

make $* 2>&1 | tee build.log

if [ -e application/Debug/bin/application.map ] && [ -e application/Debug/bin/application.nmap ]; then
    # show remaining heap size
    msp_ram=$(grep MSP_RAM application/Debug/bin/application.map | awk '{ print $2 }')
    end=$(grep " end$" application/Debug/bin/application.nmap | awk '{ printf $1}')
    echo -n "Remaining heap is "
    awk "BEGIN { print $msp_ram - 0x$end }"
fi
popd > /dev/null
if [ ${OUTPUT_BUILD} = y ]; then
	BINARY_PATH1=${PROJECT}/GCC-RELEASE/project_lp/asdk/image/
	BINARY_PATH2=${PROJECT}/GCC-RELEASE/project_hp/asdk/image/

	BINARY_PATH=${PROJECT}/GCC-RELEASE/application/Debug/bin
	export OUTPUT_FILE_LIST="${BINARY_PATH1}/km0_boot_all.bin ${BINARY_PATH2}/km4_boot_all.bin ${BINARY_PATH2}/km0_km4_image2.bin"
	export DEBUG_FILE_LIST=""
	${STDK_PATH}/tools/common/generate_output.sh
fi
