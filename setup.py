#!/usr/bin/env python
#-*- coding: utf-8 -*-

import platform
import os
import sys

REPLACE_LIST = [
	["from", "to"]
]

STDK_REF_PATH = os.path.dirname(os.path.abspath(__file__))
STDK_CORE_PATH = os.path.join(STDK_REF_PATH, "iot-core")

os.environ["STDK_REF_PATH"] = STDK_REF_PATH
os.environ["STDK_CORE_PATH"] = STDK_CORE_PATH


def print_usage():
	print("")
	print("Usage: python setup.py [BSP_NAME]")
	print("--------------------------------------------------")
	for dir in os.listdir(os.path.join(STDK_REF_PATH, "tools")):
		dirpath = os.path.join(STDK_REF_PATH, "tools", dir)
		if (os.path.exists(os.path.join(dirpath, "setup_"+ dir + ".py")) or
				os.path.exists(os.path.join(dirpath, "setup_"+ dir + ".sh"))):
			print("  ex) python setup.py " + dir)
	print("")


def find_setup_script(bsp_name):
        if "esp32" in bsp_name:
            bsp_name = "esp32"
        if os.path.exists(os.path.join("tools", bsp_name, "setup_" + bsp_name + ".py")):
                return "python " + os.path.join("tools", bsp_name, "setup_" + bsp_name + ".py")
        if "SHELL" in os.environ:
                if os.path.exists(os.path.join("tools", bsp_name, "setup_" + bsp_name + ".sh")):
                    return os.path.join("tools", bsp_name, "setup_" + bsp_name + ".sh")
        print("Fail to find setup script")
        print_usage()
        exit(1)

def update_submodule(path):
	cwd = os.getcwd()
	try:
		print("Update submodule " + path)
		os.system("git submodule sync " + path)
		os.system("git submodule init " + path)
		os.system("git submodule update " + path)
		os.chdir(path)
		os.system("git reset --hard HEAD")
	except:
		print("Failed to update submodule " + path)
	os.chdir(cwd)

def update_bsp_submodule(bsp_name):
        if "esp32" in bsp_name:
            bsp_name = "esp32"
        BSP_PATH = os.path.join(STDK_REF_PATH, "bsp", bsp_name)
        if BSP_PATH and os.path.isdir(BSP_PATH):
            update_submodule(BSP_PATH)

update_submodule(STDK_CORE_PATH)

if len(sys.argv) == 1:
	print_usage()
	exit(1)

BSP_NAME = sys.argv[1]
EXTRA_ARGS = sys.argv[2:]

for item in REPLACE_LIST:
	if BSP_NAME == item[0]:
		BSP_NAME = item[1]

setup_script = find_setup_script(BSP_NAME)
update_bsp_submodule(BSP_NAME)

setup_cmd = setup_script + " " + BSP_NAME
for args in EXTRA_ARGS:
	setup_cmd = setup_cmd + " " + args
ret_val = os.system(setup_cmd)
if ret_val:
	print_usage()
	exit(1)
else:
	print("")
	print("======================================================================")
	print("")
	print("To use SmartThings Device SDK build script, run following command:")
	print("")
	print("    python build.py " + BSP_NAME + " [APP_NAME]")
	print("")
