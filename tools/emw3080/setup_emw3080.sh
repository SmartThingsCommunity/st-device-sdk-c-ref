#!/bin/bash

CHIP_NAME=${1}

if [ ! `command -v dos2unix` ];then
	echo "Environment: dos2unix lost, please install, sudo apt-get install dos2unix"
	exit
fi

if [ -L bsp/${CHIP_NAME}/mico-os/MiCO/net/stdk/src ]; then
    rm bsp/${CHIP_NAME}/mico-os/MiCO/net/stdk/src
fi

git submodule status bsp/${CHIP_NAME} &> /dev/null
if [ "$?" == "0" ]; then
	cd bsp/${CHIP_NAME}/
	find . -type f -exec dos2unix {} &>/dev/null \;
	cd mico-os
	git add --all
	git commit -m "change dos file format to unix format with dos2unix"
	git am ../../../patches/${CHIP_NAME}/*.patch
	ln -s ../../../../../../iot-core/src/ MiCO/net/stdk/
else
	if [ "$(ls bsp/${CHIP_NAME})" == "" ]; then
		echo "Failed to find source code in bsp/${CHIP_NAME}"
	else
		cd bsp/${CHIP_NAME}/mico-os
		for patch in ../../../patches/${CHIP_NAME}/*
			do patch -f -p1 < ${patch}
		done
		ln -s ../../../../../../iot-core/src/ MiCO/net/stdk/
	fi
	echo "Check source code in bsp/${CHIP_NAME}"
fi

