#!/usr/bin/env python3

import json
import logging
import os
import shutil
import subprocess
from pathlib import Path
from tempfile import NamedTemporaryFile


VSWHERE_VERSION = "2.5.2"


def cd(path):
    logging.debug(f"change directory to '{os.path.realpath(path)}'")
    os.chdir(path)


def copy(src, dest, pattern="**/*"):
    if os.path.isdir(src):
        logging.debug(f"copy '{pattern}' from '{src}' to '{dest}'")
        os.makedirs(dest, exist_ok=True)
        for src_f in src.glob(pattern):
            dest_f = dest / src_f.relative_to(src)
            if os.path.isdir(src_f):
                os.makedirs(dest_f, exist_ok=True)
            else:
                shutil.copy2(src_f, dest_f)
    else:
        logging.debug(f"copy '{src}' to '{dest}'")
        shutil.copy2(src, dest)


def run(cmd):
    logging.debug(f"run: {cmd}")
    subprocess.check_call(cmd, shell=True)


def run_output(cmd):
    logging.debug(f"run, capturing output: {cmd}")
    return subprocess.check_output(cmd, shell=True)


def find_vs_path():
    logging.info("search VS path")
    vswhere_exe = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    vswhere_output = run_output(rf'"{vswhere_exe}" -latest -format "json" -property "installationPath"')
    vs_path = json.loads(vswhere_output)[0]["installationPath"]
    logging.info(f"found VS path: '{vs_path}'")
    return vs_path


def copy_redist(vs_path, output_dir):
    logging.info("copy redist")
    redist_path = Path(vs_path) / "VC" / "Redist" / "MSVC"
    msvc_versions = os.listdir(redist_path)
    msvc_versions.sort(key=lambda version: [int(n) for n in version.split(".")])
    copy(redist_path / msvc_versions[-1] / "vc_redist.x64.exe", output_dir)


def build_app(vs_path):
    logging.info("build solution")
    msbuild_path = Path(vs_path) / "MSBuild"
    msbuild_versions = os.listdir(msbuild_path)
    msbuild_versions.sort(key=lambda version: [int(n) if n.isdigit() else 0
                                               for n in version.split(".")])
    msbuild_exe = msbuild_path / msbuild_versions[-1] / "Bin" / "MSBuild"
    run(rf'"{msbuild_exe}" /m /v:m /p:Configuration=Release /p:Platform=x64')


def copy_app(output_dir):
    logging.info("copy app")
    copy(Path("build") / "x64-Release", output_dir, "*.dll")
    copy(Path("build") / "x64-Release", output_dir, "*.exe")


if __name__ == "__main__":
    logging.basicConfig(datefmt="%H:%M:%S",
                        format="[%(asctime)s][%(levelname)s] %(message)s",
                        level=logging.DEBUG)
    cd(os.path.dirname(os.path.abspath(__file__)))

    package_dir = Path("build") / "package"
    if os.path.exists(package_dir):
        shutil.rmtree(package_dir)
    os.makedirs(package_dir)

    vs_path = find_vs_path()
    copy_redist(vs_path, package_dir)
    build_app(vs_path)
    copy_app(package_dir)
