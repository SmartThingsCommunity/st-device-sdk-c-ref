#!/usr/bin/env python

import platform
import sys
import os
import subprocess

def is_valid_submodule(target_path):
    working_dir = os.getcwd()
    os.chdir(os.path.dirname(working_dir))
    target_commit_id = subprocess.check_output(['git','ls-files','-s', target_path]).split(' ')[1][0:7]
    os.chdir(target_path)
    number_of_patches = len(subprocess.check_output(['git','log', '--branches', '--not', '--remotes', '--pretty=oneline']).splitlines())
    target_commit_list = subprocess.check_output(['git', 'log', '-n'+str(number_of_patches+1), '--pretty=oneline']).splitlines()
    matched_line = [line for line in target_commit_list if line.startswith(target_commit_id)]
    os.chdir(working_dir)
    if matched_line:
        return 1
    else:
        return 0

BSP_PATH = sys.argv[1]
    
if not is_valid_submodule(BSP_PATH) :
    print("********************************************************")
    print("  WARN: Unexpected submodule state : ")
    print("        (" + BSP_PATH + ")")
    print("        Please check submodule or use 'setup.py'")
    print("********************************************************")

