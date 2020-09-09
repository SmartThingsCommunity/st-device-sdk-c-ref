#!/bin/bash

WARN_MSG="WARN : This script will be DEPRECATED. Please use 'setup.py'"

export STDK_REF_PATH=$(dirname $(realpath $0))
export STDK_CORE_PATH="${STDK_REF_PATH}/iot-core"

CORE_PATH="iot-core"
BSP_NAME=${1}
ARGUMENTS=$@



print_usage () {
	echo "    Usage: ./setup.sh BSP_NAME"
	echo "- - - - - - - - - - - - - - - - - - -"
	echo "    ex) ./setup.sh esp8266"
	echo "    ex) ./setup.sh esp32"
	echo "    ex) ./setup.sh rtl8195"
	echo "    ex) ./setup.sh rtl8720c"
	echo "    ex) ./setup.sh rtl8721c"
	echo "    ex) ./setup.sh emw3166"
	echo "    ex) ./setup.sh emw3080"
	echo
}

if [ "${BSP_NAME}" == "" ]; then
	print_usage
	echo ${WARN_MSG}
	exit 0
fi

if [ ! -e tools/${BSP_NAME}/setup_${BSP_NAME}.sh ]; then
	echo "Failed to find tools/${BSP_NAME}/setup_${BSP_NAME}.sh"
	print_usage
	echo ${WARN_MSG}
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

git submodule status bsp/${BSP_NAME} &> /dev/null
if [ "$?" == "0" ]; then
	git submodule sync bsp/${BSP_NAME}
	git submodule init bsp/${BSP_NAME}
	git submodule update bsp/${BSP_NAME}
	cd bsp/${BSP_NAME}
	git reset --hard HEAD
	cd ../..
fi

tools/${BSP_NAME}/setup_${BSP_NAME}.sh ${ARGUMENTS}
if [ "$?" == "0" ]; then
  echo ""
  echo "To use SmartThings Device SDK build script, run following command:"
  echo ""
  echo "    python build.py ${BSP_NAME} [APP_NAME]"
  echo ""
fi
echo ${WARN_MSG}

