#!/usr/bin/env python3
import os
import sys
import subprocess
from pathlib import Path

#!/usr/bin/env python3
import os
import sys
import subprocess
from pathlib import Path

def validate_args():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <BSP_NAME> <PROJECT_TITLE> [COMMAND] [PORT]")
        print("Available commands: build(default), clean, erase_flash, flash, monitor")
        sys.exit(1)

    bsp_name = sys.argv[1]
    project_title = sys.argv[2]
    command = sys.argv[3] if len(sys.argv) >=4 else "build"
    
    port = None
    if command in ("erase_flash", "flash", "monitor"):
        if len(sys.argv) < 5:
            print(f"Error: {command} command requires port argument")
            sys.exit(1)
        port = sys.argv[4]
    elif len(sys.argv) >=5:
        print(f"Warning: Extra argument '{sys.argv[4]}' will be ignored")

    return bsp_name, project_title, command, port

def setup_paths(bsp_name, project_title):
    stdk_path = Path.cwd().resolve()
    return {
        "project_path": stdk_path / "apps" / bsp_name / project_title,
        "build_output_path": stdk_path / "apps" / bsp_name / project_title / "build_out",
        "flash_tool": stdk_path / "bsp" / bsp_name / "tools/flash_tool/bflb_iot_tool-ubuntu",
        "partition_table": stdk_path / "bsp" / bsp_name / "tools/flash_tool/chips/bl602/partition/partition_cfg_2M.toml",
        "app_firmware": stdk_path / "apps" / bsp_name / project_title / "build_out" / f"{project_title}.bin",
        "monitor_script": stdk_path / "tools" / bsp_name / "monitor.py",
        "baudrate": "2000000"
    }

def build_project(paths):
    make_cmd = [
        "make",
        "CONFIG_CHIP_NAME=BL602",
        "CONFIG_LINK_ROM=1",
        "CONFIG_BLE_TP_SERVER=1",
        "CONFIG_BLECONTROLLER_LIB=all",
        f"-j{os.cpu_count() or 4}",
        "PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC=1",
        "CONFIG_BT_STACK_PTS=1"
    ]
    subprocess.run(make_cmd, check=True)

def execute_command(command, paths, port=None):
    os.chdir(paths["project_path"])
    
    command_handlers = {
        "build": lambda: build_project(paths),
        "clean": lambda: subprocess.run(["rm", "-rf", str(paths["build_output_path"])], check=True) if paths["build_output_path"].exists() else None,
        "erase_flash": lambda: subprocess.run([
            str(paths["flash_tool"]),
            "--chipname=BL602",
            f"--baudrate={paths['baudrate']}",
            f"--port={port or '/dev/ttyACM0'}",
            f"--pt={paths['partition_table']}",
            "--dts=",
            f"--firmware={paths['app_firmware']}",
            "--erase"
        ], check=True),
        "flash": lambda: subprocess.run([
            str(paths["flash_tool"]),
            "--chipname=BL602",
            f"--baudrate={paths['baudrate']}",
            f"--port={port or '/dev/ttyACM0'}",
            f"--pt={paths['partition_table']}",
            "--dts=",
            f"--firmware={paths['app_firmware']}"
        ], check=True) if (paths["build_output_path"].exists() or build_project(paths)) else None,
        "monitor": lambda: subprocess.run([
            "/usr/bin/python3",
            str(paths["monitor_script"]),
            port
        ], check=True) if port else None
    }

    if handler := command_handlers.get(command):
        handler()
    else:
        print(f"Invalid command: {command}")
        sys.exit(1)

def main():
    try:
        bsp_name, project_title, command, port = validate_args()
        paths = setup_paths(bsp_name, project_title)
        execute_command(command, paths, port)
    except subprocess.CalledProcessError as e:
        print(f"\nCommand failed with code {e.returncode}:")
        print(" ".join(e.cmd))
        sys.exit(1)
    except Exception as e:
        print(f"\nError: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    main()