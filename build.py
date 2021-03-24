#!/usr/bin/env python
#-*- coding: utf-8 -*-

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
    print("Usage: python build.py apps/[BSP_NAME]/[APP_NAME]")
    print("                 or")
    print("       python build.py [BSP_NAME] [APP_NAME]")
    print("--------------------------------------------------")

    try:
        usage_buf = ""
        for bsp_dir in os.listdir(os.path.join(STDK_REF_PATH, "apps")):
            if (os.path.isdir(os.path.join(STDK_REF_PATH, "apps", bsp_dir))
                   and os.path.isdir(os.path.join(STDK_REF_PATH, "apps", bsp_dir))):
                app_list = ""
                for dir in os.listdir(os.path.join(STDK_REF_PATH, "apps", bsp_dir)):
                    if os.path.isdir(os.path.join(STDK_REF_PATH, "apps", bsp_dir, dir)):
                        app_list = app_list + dir + ", "
                if app_list:
                    usage_buf = usage_buf + "  " + format(bsp_dir, "12s") + ": " + app_list[:-2] + "\n"
    except:
        pass
    else:
        print("   [BSP_NAME] : [APP_NAME]")
        print(usage_buf)
    finally:
        print("  ex) python build.py apps/esp32/switch_example")
        print("  ex) python build.py esp32 light_example")
    print("")

def find_build_script(bsp_name):
    if os.path.exists(os.path.join("tools", bsp_name, "build_"+bsp_name+".py")):
        return "python " + os.path.join("tools", bsp_name, "build_"+bsp_name+".py")
    if "SHELL" in os.environ:
        if os.path.exists(os.path.join("tools", bsp_name, "build_"+bsp_name+".sh")):
            return os.path.join("tools", bsp_name, "build_"+bsp_name+".sh")
    print("Fail to find build script")
    print_usage()
    exit(1)

if len(sys.argv) == 1:
    print_usage()
    exit(1)


if os.path.exists(sys.argv[1]):
    # assume that input type is apps/[BSP_NAME]/[APP_NAME]
    BSP_NAME = os.path.split(os.path.split(os.path.abspath(sys.argv[1]))[0])[1]
    APP_NAME = os.path.split(os.path.abspath(sys.argv[1]))[1]
    EXTRA_ARGS = sys.argv[2:]
else:
    if len(sys.argv) < 3:
        print("Error : Fail to find app path")
        print_usage()
        exit(1)
    BSP_NAME = sys.argv[1]
    APP_NAME = sys.argv[2]
    EXTRA_ARGS = sys.argv[3:]

for item in REPLACE_LIST:
    if BSP_NAME == item[0]:
        BSP_NAME = item[1]

build_script = find_build_script(BSP_NAME)
build_cmd = build_script + " " + BSP_NAME + " " + APP_NAME
for args in EXTRA_ARGS:
    build_cmd = build_cmd + " " + args
ret_val = os.system(build_cmd)
if ret_val:
    print_usage()
    exit(1)
