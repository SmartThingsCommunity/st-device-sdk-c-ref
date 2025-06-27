#!/usr/bin/env python3
import os
import sys
import re
import subprocess
from pathlib import Path

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <BSP_NAME> <PROJECT_TITLE> [clean|erase_flash|flash|monitor] [port]")
        print("Available actions: clean, erase_flash, flash, monitor")
        sys.exit(1)

    # Parse base parameters
    bsp_name = sys.argv[1]
    project_title = sys.argv[2]

    # Handle optional parameters
    action = sys.argv[3] if len(sys.argv) >=4 else None
    uart_device = sys.argv[4] if len(sys.argv) >=5 else None

    # Configure paths
    stdk_path = Path(os.getcwd())
    bsp_path = stdk_path / "bsp" / bsp_name
    build_output_path = bsp_path / "build" / project_title
    flash_tool_path = stdk_path / "tools" / bsp_name
    bin_file = build_output_path / bsp_name / "all-app.bin"

    def check_uart_permission():
        if not uart_device:
            print("Error: No serial device specified", file=sys.stderr)
            sys.exit(1)

        uart_path = Path(uart_device)
        if not uart_path.exists():
            print(f"Error: Serial device not found [{uart_device}]", file=sys.stderr)
            print("Available devices:", file=sys.stderr)
            subprocess.run(["ls", "/dev/tty*"], stderr=sys.DEVNULL, stdout=sys.DEVNULL)
            sys.exit(1)

        if not uart_path.is_char_device():
            print(f"Error: Not a character device [{uart_device}]", file=sys.stderr)
            sys.exit(1)

        if not os.access(uart_device, os.R_OK | os.W_OK):
            print(f"Error: Missing read/write permissions for [{uart_device}]", file=sys.stderr)
            print("Try these solutions:", file=sys.stderr)
            print(f"1. Temporary solution (valid until reboot):\n   sudo chmod 666 {uart_device}", file=sys.stderr)
            print(f"2. Permanent solution:\n   sudo usermod -aG dialout {os.getenv('USER')}", file=sys.stderr)
            print("   Requires re-login to take effect", file=sys.stderr)
            sys.exit(1)

    def check_bin_file():
        if not bin_file.exists():
            print(f"Error: Firmware file not found [{bin_file}]", file=sys.stderr)
            print("Please compile the project first", file=sys.stderr)
            sys.exit(1)

    try:
        os.chdir(bsp_path)

        if not action:  # 默认build操作
            project_path = Path("apps") / bsp_name / project_title
            subprocess.run(
                ["make", bsp_name, f"PROJECT=../../../{project_path}"],
                check=True
            )
        elif action == "clean":
            subprocess.run(["rm", "-rf", str(build_output_path)], check=True)
        elif action == "erase_flash":
            check_uart_permission()
            uart_port = re.findall(r'\d+$', uart_device)[-1]
            subprocess.run(
                [str(flash_tool_path / "bk_loader"), "erase", "-p", uart_port, "-c"],
                cwd=flash_tool_path,
                check=True
            )
            
        elif action == "flash":
            check_bin_file()
            check_uart_permission()
            uart_port = re.findall(r'\d+$', uart_device)[-1]
            subprocess.run(
                [str(flash_tool_path / "bk_loader"), "download", "-p", uart_port, "-i", str(bin_file)],
                cwd=flash_tool_path,
                check=True
            )
            
        elif action == "monitor":
            check_uart_permission()
            monitor_script = stdk_path / "tools" / bsp_name / "monitor.py"
            subprocess.run(
                [sys.executable, str(monitor_script), uart_device],
                check=True
            )
            
        else:  # Default build action
            project_path = Path("apps") / bsp_name / project_title
            subprocess.run(
                ["make", bsp_name, f"PROJECT=../../../{project_path}"],
                check=True
            )

    except subprocess.CalledProcessError as e:
        print(f"Command failed with code {e.returncode}", file=sys.stderr)
        sys.exit(e.returncode)
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
