#!/usr/bin/env python3

import platform
import sys
import os
import shutil
import subprocess

OUTPUT_OPTION_ARRAY=["all", "flash", "app", "app-flash", "bootloader", "bootloader-flash", "partition_table"]

BSP_NAME = sys.argv[1]
APP_NAME = sys.argv[2]
EXTRA_ARGS = sys.argv[3:]

BSP_PATH = os.path.join(os.environ["STDK_REF_PATH"], "bsp", BSP_NAME)
APP_PATH = os.path.join(os.environ["STDK_REF_PATH"], "apps", "esp32", APP_NAME)

STDK_PATH = os.environ["STDK_REF_PATH"]
CORE_PATH = os.environ["STDK_CORE_PATH"]

os.environ["IDF_PATH"] = BSP_PATH

print("PATH : " + os.environ["PATH"])

if not (os.path.exists(BSP_PATH) and os.path.exists(APP_PATH)):
    print("Fail to find path.")
    exit(1)

shutil.copy(os.path.join(CORE_PATH, "src", "Kconfig"), os.path.join(BSP_PATH, "components", "iot-core_config"))

if EXTRA_ARGS:
    MAKE_OPTION = ""
    for args in EXTRA_ARGS:
        MAKE_OPTION = MAKE_OPTION + " " + args
else:
    MAKE_OPTION = "build"

os.chdir(BSP_PATH)
if "SHELL" in os.environ:
    export_cmd = ". ./export.sh"
else:
    export_cmd = "export.bat"

build_cmd = "python " + os.path.join(BSP_PATH, "tools", "idf.py") + " " + MAKE_OPTION

subprocess.call(export_cmd + " && cd " + APP_PATH + " && " + build_cmd, shell=True)
