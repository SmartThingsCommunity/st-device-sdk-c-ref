#!/usr/bin/env python3

import platform
import sys
import os
import shutil
import subprocess

BSP_NAME = sys.argv[1]
BSP_PATH = os.path.join(os.environ["STDK_REF_PATH"], "bsp", BSP_NAME)
PATCH_PATH = os.path.join(os.environ["STDK_REF_PATH"], "patches", BSP_NAME)

CONFIG_PATH = os.path.join(BSP_PATH, "components", "iot-core_config")


os.chdir(BSP_PATH)
os.system("git am " + os.path.join(PATCH_PATH, "*.patch"))
if not os.path.exists(CONFIG_PATH):
	os.makedirs(CONFIG_PATH)
cmake_file = open(os.path.join(CONFIG_PATH, "CMakeLists.txt"), "w")
cmake_file.write("idf_component_register()")
cmake_file.close()
'''
elif platform.system() == "Windows":
	print("Failed to apply patches")
	exit(1)
else:
	if os.listdir(BSP_PATH) == []:
		print("Failed to find source code in " + BSP_PATH)
		exit(1)
	else:
		os.chdir(BSP_PATH)
		patch_files = os.listdir(PATCH_PATH)
		for patch_file_name in patch_files:
			os.system("patch -f -p1 < " + os.path.join(PATCH_PATH, patch_file_name))
		print("Check source code in " + BSP_PATH)
		exit(0)
'''
os.chdir(BSP_PATH)
if platform.system() == "Windows":
	os.system("install.bat")
else:
	os.system("./install.sh")
