#!/usr/bin/env python

import platform
import sys
import os
import subprocess

def is_valid_submodule(bsp_path, patch_path):
    working_dir = os.getcwd()

    os.chdir(patch_path)
    number_of_patches = len([file for file in os.listdir(patch_path) if file.endswith("patch")])
    os.chdir(working_dir)

    os.chdir(os.path.dirname(bsp_path))
    bsp_commit_id = subprocess.check_output(['git', 'ls-files', '-s', bsp_path], universal_newlines=True).split(' ')[1][0:7]
    os.chdir(working_dir)

    os.chdir(bsp_path)
    target_commit_list = subprocess.check_output(['git', 'log', '-n'+str(number_of_patches+1), '--pretty=oneline'], universal_newlines=True).splitlines()
    matched_line = [line for line in target_commit_list if line.startswith(bsp_commit_id)]
    os.chdir(working_dir)

    if matched_line:
        return 1
    else:
        return 0

def main():
    try:
        BSP_PATH = os.path.abspath(sys.argv[1])
        PATCH_PATH = os.path.abspath(sys.argv[2])

        if not is_valid_submodule(BSP_PATH, PATCH_PATH):
            print("WARN: Unexpected submodule state : ")
            print("      (" + BSP_PATH + ")")
            print("      If a problem occurs, Please check submodule or use 'setup.py'")
    except:
        print("WARN: Failed to check submodule state.")
        print("      If a problem occurs, Please check submodule or use 'setup.py'")
        exit(0)