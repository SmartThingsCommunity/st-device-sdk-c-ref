#!/usr/bin/env python

import platform
import sys
import os
import shutil
import subprocess

OUTPUT_OPTION_ARRAY=["all", "build", "flash", "app", "app-flash", "bootloader", "bootloader-flash", "partition_table"]
AFTER_OPTION_ARRAY=["flash", "monitor"]

BSP_NAME = sys.argv[1]
APP_NAME = sys.argv[2]
EXTRA_ARGS = sys.argv[3:]

BSP_PATH = os.path.join(os.environ["STDK_REF_PATH"], "bsp", BSP_NAME)
PATCH_PATH = os.path.join(os.environ["STDK_REF_PATH"], "patches", BSP_NAME)
APP_PATH = os.path.join(os.environ["STDK_REF_PATH"], "apps", "esp32", APP_NAME)
COMMON_TOOLS_PATH = os.path.join(os.environ["STDK_REF_PATH"], "tools", "common")

os.environ["IDF_PATH"] = BSP_PATH


def get_qrgen_image():
    qrgen_script = os.path.join(os.environ["STDK_REF_PATH"], "iot-core", "tools", "qrgen", "stdk-qrgen.py")
    qrgen_image = ""
    if os.path.isfile(qrgen_script):
        try:
            import qrcode
        except:
            print("WARN: Failed to import 'qrcode' to generate QR image for easysetup.")
            print("      To install module, run 'pip install qrcode'")
        else:
            try:
                qrgen_output = subprocess.check_output(
                    ["python", qrgen_script, "--folder", os.path.join(APP_PATH, "main")], universal_newlines=True)
            except:
                print("WARN: Failed to generate QR image for easysetup.")
            else:
                if "Error" in qrgen_output:
                    print(qrgen_output)
                    print("WARN: Failed to generate QR image for easysetup.")
                    if "No module named" in qrgen_output:
                        print("      To install module, run 'pip install " + qrgen_output.split()[-1] +"'")
                else:
                    qrgen_image = qrgen_output.split()[1]
                    qrgen_url = qrgen_output.split()[4]
                    if not qrgen_image.endswith(".png"):
                        qrgen_image = ""
                    elif "m=MNID" in qrgen_url or "r=serialNumber_here" in qrgen_url:
                        # invalid file name or default json file
                        os.remove(qrgen_image)
                        qrgen_image = ""
                    else:
                        qrgen_image = os.path.abspath(qrgen_image)
    return qrgen_image

def copy_output():
    # generate output
    with open(os.path.join(APP_PATH, "CMakeLists.txt"), 'r') as readfile:
        lines = readfile.readlines()
        for line in lines:
            if line.startswith("project"):
                project_name = line.replace(')', '(').split('(')[1]

    use_custom_partition_table = False
    with open(os.path.join(APP_PATH, "sdkconfig"), 'r') as readfile:
        lines = readfile.readlines()
        for line in lines:
            if line.startswith("CONFIG_PARTITION_TABLE_CUSTOM=y"):
                use_custom_partition_table = True
            elif line.startswith("CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="):
                custom_partition_name = line.replace('"', '=').split('=')[2]
            elif line.startswith("CONFIG_PARTITION_TABLE_FILENAME="):
                partition_name = line.replace('"', '=').split('=')[2]
    if use_custom_partition_table:
        partition_name = custom_partition_name

    output_list = [
        os.path.join(APP_PATH, "build", "ota_data_initial.bin"),
        os.path.join(APP_PATH, "build", "bootloader", "bootloader.bin"),
        os.path.join(APP_PATH, "build", project_name + ".bin"),
        os.path.join(APP_PATH, "build", "partition_table", "partition-table.bin"),
    ]

    qrgen_image = get_qrgen_image()
    if qrgen_image:
        output_list.append(os.path.join(qrgen_image))

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

ret = subprocess.call(export_cmd + " && cd " + APP_PATH + " && " + build_cmd, shell=True)
if "clean" in MAKE_OPTION.split(' '):
    print("\nTip : To remove all previous build information,")
    print("      'fullclean' is recommended instead of 'clean'.")
    print("      For more information, run script with '--help' option.\n")
if ret != 0:
    exit(1)


if set(MAKE_OPTION.split()).intersection(OUTPUT_OPTION_ARRAY):
    copy_output()

subprocess.call("python " + os.path.join(COMMON_TOOLS_PATH, "check_submodule.py") + " " + BSP_PATH + " " + PATCH_PATH, shell=True)
