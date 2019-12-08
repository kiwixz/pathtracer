#!/usr/bin/env python3

import logging
import os
import platform
import shutil
import subprocess
import sys
from distutils import dir_util
from pathlib import Path


def is_win():
    return platform.system() == "Windows"


def install_vcpkg(vcpkg_ref):
    logging.info("installing vcpkg")
    if Path("vcpkg/.git").exists():
        logging.warning("vcpkg seems already installed, trying to update")
        subprocess.check_call(["git", "-C", "vcpkg", "reset", "--hard"])
        subprocess.check_call(["git", "-C", "vcpkg", "clean", "-df"])
        subprocess.check_call(["git", "-C", "vcpkg", "fetch", "-p"])
        if (subprocess.check_output(["git", "-C", "vcpkg", "rev-parse", "HEAD"])
                == subprocess.check_output(["git", "-C", "vcpkg", "rev-parse", vcpkg_ref])):
            logging.warning("vcpkg seems already up-to-date, skipping")
            return
    else:
        subprocess.check_call(["git", "clone", "https://github.com/Microsoft/vcpkg"])
    subprocess.check_call(["git", "-C", "vcpkg", "checkout", vcpkg_ref])
    if is_win():
        subprocess.check_call([str(Path("vcpkg/bootstrap-vcpkg.bat")), "-disableMetrics"])
    else:
        subprocess.check_call([str(Path("vcpkg/bootstrap-vcpkg.sh")), "-disableMetrics", "-useSystemBinaries"])


def install_vcpkg_packages(libs):
    logging.info("installing vcpkg libs")
    triplet = "x64-windows" if is_win() else "x64-linux"
    vcpkg_cmd = [str(Path("vcpkg/vcpkg"))]
    custom_triplets = Path("vcpkg_config/triplets").resolve()
    if custom_triplets.exists():
        vcpkg_cmd.append(f"--overlay-triplets={str(custom_triplets)}")
    custom_ports = Path("vcpkg_config/ports").resolve()
    if custom_ports.exists():
        vcpkg_cmd.append(f"--overlay-ports={str(custom_ports)}")

    subprocess.check_call(vcpkg_cmd + ["remove", "--outdated", "--no-purge", "--recurse"])
    subprocess.check_call(vcpkg_cmd + ["install", "--recurse", "--triplet", triplet] + libs)


def cleanup_vcpkg():
    logging.info("cleaning up vcpkg")
    vcpkg_path = Path("vcpkg")
    shutil.rmtree(vcpkg_path / "packages", True)
    for lib in (vcpkg_path / "buildtrees").glob("*"):
        for dir in lib.glob("*"):
            if dir.is_dir() and dir.name != "src":  # keep source for debugging
                shutil.rmtree(str(dir))
    for file in (vcpkg_path / "downloads").glob("*"):
        if file.is_file():
            file.unlink()


if __name__ == "__main__":
    logging.basicConfig(datefmt="%H:%M:%S",
                        format="[%(asctime)s][%(levelname)s] %(message)s",
                        level=logging.INFO,
                        stream=sys.stderr)
    os.chdir(str(Path(__file__).parent.parent))

    vcpkg_ref = sys.argv[1]
    libs = sys.argv[2:]

    if vcpkg_ref:
        install_vcpkg(vcpkg_ref)
    if len(libs) > 0:
        install_vcpkg_packages(libs)
    cleanup_vcpkg()
