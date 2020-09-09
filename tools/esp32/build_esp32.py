#!/usr/bin/env python

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
COMMON_TOOLS_PATH = os.path.join(os.environ["STDK_REF_PATH"], "tools", "common")

os.environ["IDF_PATH"] = BSP_PATH

if not (os.path.exists(BSP_PATH) and os.path.exists(APP_PATH)):
    print("Fail to find path.")
    exit(1)

shutil.copy(os.path.join(os.environ["STDK_CORE_PATH"], "src", "Kconfig"), os.path.join(BSP_PATH, "components", "iot-core_config", "Kconfig"))

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

#generate output
with open(os.path.join(APP_PATH, "CMakeLists.txt"), 'r') as readfile:
    lines = readfile.readlines()
    for line in lines:
        if line.startswith("project"):
            project_name= line.replace(')','(').split('(')[1]

use_custom_partition_table = False
with open(os.path.join(APP_PATH, "sdkconfig"), 'r') as readfile:
     lines = readfile.readlines()
     for line in lines:
         if line.startswith("CONFIG_PARTITION_TABLE_CUSTOM=y"):
             use_custom_partition_table = True
         elif line.startswith("CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="):
             custom_partition_name = line.replace('"','=').split('=')[2]
         elif line.startswith("CONFIG_PARTITION_TABLE_FILENAME="):
             partition_name = line.replace('"','=').split('=')[2]
if use_custom_partition_table:
    partition_name = custom_partition_name

qr_file =  os.path.join(APP_PATH, project_name + ".png")
subprocess.call("python " + os.path.join(COMMON_TOOLS_PATH, "generate_qr.py") + " " + APP_PATH + " " + qr_file, shell=True)

output_list = [
    os.path.join(APP_PATH, "build", "ota_data_initial.bin"),
    os.path.join(APP_PATH, "build", "bootloader", "bootloader.bin"),
    os.path.join(APP_PATH, "build", project_name + ".bin"),
    os.path.join(APP_PATH, "build", "partition_table", "partition-table.bin"),
]

if os.path.isfile(qr_file):
    output_list.append(qr_file)

debug_list = [
    os.path.join(APP_PATH, "build", project_name + ".elf"),
    os.path.join(APP_PATH, "build", project_name + ".map"),
    os.path.join(APP_PATH, "build", "bootloader", "bootloader.elf"),
    os.path.join(APP_PATH, "build", "bootloader", "bootloader.map"),
    os.path.join(APP_PATH, "sdkconfig"),
    ]

sys.path.append(COMMON_TOOLS_PATH)
from generate_output import generate_output
generate_output(os.environ["STDK_REF_PATH"], BSP_NAME, APP_NAME, output_list, debug_list)

if os.path.isfile(qr_file):
    os.remove(qr_file)

subprocess.call("python " + os.path.join(COMMON_TOOLS_PATH, "check_submodule.py") + " " + BSP_PATH, shell=True)
