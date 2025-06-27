#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path
from contextlib import contextmanager

def validate_env():
    """Verify required environment variables"""
    stdk_ref_path = os.environ.get("STDK_REF_PATH")
    if not stdk_ref_path:
        raise EnvironmentError("STDK_REF_PATH environment variable not set")
    return Path(stdk_ref_path)

@contextmanager
def change_directory(path: Path):
    """Context manager for directory changes"""
    origin = Path.cwd()
    try:
        os.chdir(str(path))
        yield
    finally:
        os.chdir(str(origin))

def check_git_submodule(stdk_ref: Path, bsp_name: str) -> bool:
    """Check if it's a git submodule"""
    cmd = [
        "git", "-C", str(stdk_ref),
        "submodule", "status", f"bsp/{bsp_name}"
    ]
    result = subprocess.run(
        cmd, 
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )
    return result.returncode == 0

def apply_patch(target_dir: Path, patch_dir: Path, is_git: bool):
    """Apply patch files"""
    print(f"Applying patches to: {target_dir}")
    
    if not patch_dir.is_dir():
        print(f"Warning: Patch directory not found {patch_dir}")
        return

    patches = list(patch_dir.glob("*.patch"))
    if not patches:
        print(f"Info: No valid patch files in {patch_dir}")
        return

    with change_directory(target_dir):
        for patch in patches:
            print(f"Processing patch: {patch.name}")
            try:
                if is_git:
                    subprocess.run(
                        ["git", "apply", str(patch)],
                        check=True
                    )
                else:
                    subprocess.run(
                        ["patch", "-f", "-p1", "<", str(patch)],
                        check=True,
                        shell=True  # Required for redirection
                    )
            except subprocess.CalledProcessError as e:
                print(f"Patch application failed: {e}")
                sys.exit(1)

def main(bsp_name: str):
    stdk_ref = validate_env()
    
    # Build paths
    bsp_path = stdk_ref / "bsp" / bsp_name
    patch_path = stdk_ref / "patches" / bsp_name
    core_path = stdk_ref / "iot-core"
    libsodium_path = core_path / "src/deps/libsodium/libsodium"

    # Validate critical paths
    if not all([bsp_path.exists(), libsodium_path.exists()]):
        raise FileNotFoundError("Critical paths missing, please check environment configuration")

    # Check submodule status
    is_git = check_git_submodule(stdk_ref, bsp_name)

    # Process libsodium
    with change_directory(libsodium_path):
        subprocess.run(
            ["git", "submodule", "update", "--init", "--recursive"], 
            check=True
        )
        subprocess.run(
            ["git", "submodule", "foreach", "--recursive", "git", "reset", "--hard"],
            check=True
        )

    # Apply patches
    apply_patch(bsp_path, patch_path, is_git)

    # Execute environment setup
    env_setup_script = bsp_path / "tools/env_tools/setup/armino_env_setup.sh"
    if not env_setup_script.exists():
        raise FileNotFoundError(f"Environment setup script not found: {env_setup_script}")
    
    try:
        subprocess.run(
            ["sudo", "bash", str(env_setup_script)],
            check=True
        )
    except PermissionError:
        print("Root privileges required, please configure passwordless sudo or execute manually")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <BSP_NAME>")
        sys.exit(1)
    
    try:
        main(sys.argv[1])
    except Exception as e:
        print(f"Execution error: {str(e)}")
        sys.exit(1)