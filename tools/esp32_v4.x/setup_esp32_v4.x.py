#!/usr/bin/env python

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

os.chdir(BSP_PATH)
if "SHELL" in os.environ:
	os.system("./install.sh")
else:
	os.system("install.bat")