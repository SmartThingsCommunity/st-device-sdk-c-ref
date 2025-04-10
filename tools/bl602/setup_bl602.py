#!/usr/bin/env python

import sys
import os
import shutil

BSP_NAME = sys.argv[1]
CORE_PATH = os.path.join(os.environ["STDK_CORE_PATH"])
BSP_PATH = os.path.join(os.environ["STDK_REF_PATH"], "bsp", "bl602")
PATCH_PATH = os.path.join(os.environ["STDK_REF_PATH"], "patches", "bl602")
LIBSODIUM_PATH = os.path.join(os.environ["STDK_CORE_PATH"], "src", "deps", "json", "cJSON")
JSON_PATH = os.path.join(os.environ["STDK_CORE_PATH"], "src", "deps", "libsodium", "libsodium")

os.chdir(LIBSODIUM_PATH)
os.system("git submodule update --init --recursive")
os.system("git submodule foreach --recursive git reset --hard")

os.chdir(JSON_PATH)
os.system("git submodule update --init --recursive")
os.system("git submodule foreach --recursive git reset --hard")

os.chdir(BSP_PATH)
os.system("git submodule update --init --recursive")
os.system("git submodule foreach --recursive git reset --hard")

if os.path.isdir(PATCH_PATH):
    os.chdir(BSP_PATH)
    for patchfile in sorted(os.listdir(PATCH_PATH)):
        if patchfile.endswith(".patch"):
            os.system("git apply " + os.path.join(PATCH_PATH, patchfile))


