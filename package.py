#!/usr/bin/env python3

import json
import logging
import os
import re
import shutil
import subprocess
import tempfile
import zipfile
from pathlib import Path


APP_NAME = "pathtracer"
OUTPUT_FILE = f"build/{APP_NAME}.zip"
VSWHERE_VERSION = "2.5.2"


# actions


def cd(path):
    logging.debug(f"change directory to '{os.path.realpath(path)}'")
    os.chdir(path)


def copy(src, dest):
    logging.debug(f"copy '{src}' to '{dest}'")
    shutil.copy2(src, dest)


def copy_all(src, dest, pattern="**/*"):
    logging.debug(f"copy '{pattern}' from '{src}' to '{dest}'")
    os.makedirs(dest, exist_ok=True)
    for src_f in src.glob(pattern):
        dest_f = dest / src_f.relative_to(src)
        if os.path.isdir(src_f):
            os.makedirs(dest_f, exist_ok=True)
        else:
            shutil.copy2(src_f, dest_f)


def run(cmd):
    logging.debug(f"run: {cmd}")
    subprocess.check_call(cmd, shell=True)


def run_output(cmd):
    logging.debug(f"run, capturing output: {cmd}")
    return subprocess.check_output(cmd, shell=True)


# helpers


def last_version(path):
    versions = os.listdir(path)
    versions.sort(key=lambda version: [int(n) if n.isdigit() else 0
                                       for n in version.split(".")])
    return path / versions[-1]


# steps


def find_vs_path():
    logging.info("search VS path")
    vswhere_exe = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    vswhere_output = run_output(rf'"{vswhere_exe}" -latest -format "json" -property "installationPath"')
    vs_path = json.loads(vswhere_output)[0]["installationPath"]
    logging.info(f"found VS path: '{vs_path}'")
    return vs_path


def find_required_dlls(package_dir):
    logging.info("search required DLLs")
    tools_path = Path(vs_path) / "VC" / "Tools" / "MSVC"
    dumpbin_exe = last_version(tools_path) / "bin" / "Hostx64" / "x64" / "dumpbin.exe"
    dlls_list = set()
    for exe in package_dir.glob("**/*.exe"):
        raw = run_output(rf'"{dumpbin_exe}" /DEPENDENTS {exe}')
        matches = re.findall(r'(^|\s)([\w.-]+\.dll)($|\s)', raw.decode("utf8"))
        dlls_list = dlls_list.union([match[1] for match in matches])
    return dlls_list


def copy_redist(vs_path, package_dir):
    required_dlls = [dll.lower() for dll in find_required_dlls(package_dir)]
    logging.info("copy redist")
    redist_path = Path(vs_path) / "VC" / "Redist" / "MSVC"
    redist_path = last_version(redist_path) / "x64"
    for dll in redist_path.glob("**/*"):
        if dll.name.lower() in required_dlls:
            copy(dll, package_dir)


def build_app(vs_path):
    logging.info("build solution")
    msbuild_path = Path(vs_path) / "MSBuild"
    msbuild_exe = last_version(msbuild_path) / "Bin" / "MSBuild.exe"
    run(rf'"{msbuild_exe}" /m /v:m /p:Configuration=Release /p:Platform=x64')


def copy_app(package_dir):
    logging.info(f"copy {APP_NAME}")
    copy_all(Path("build") / "x64-Release", package_dir, "*.dll")
    copy_all(Path("build") / "x64-Release", package_dir, "*.exe")


def zip_app(package_dir):
    logging.info(f"zip {APP_NAME}")
    with zipfile.ZipFile(OUTPUT_FILE, "w", zipfile.ZIP_LZMA) as zipf:
        for f in package_dir.glob("**/*"):
            if os.path.isfile(f):
                zipf.write(f, f"{APP_NAME}/{f.relative_to(package_dir)}")


# main


if __name__ == "__main__":
    logging.basicConfig(datefmt="%H:%M:%S",
                        format="[%(asctime)s][%(levelname)s] %(message)s",
                        level=logging.DEBUG)
    cd(os.path.dirname(os.path.abspath(__file__)))

    package_dir = Path(tempfile.mkdtemp())
    try:
        vs_path = find_vs_path()
        build_app(vs_path)
        copy_app(package_dir)
        copy_redist(vs_path, package_dir)
        zip_app(package_dir)
    finally:
        shutil.rmtree(package_dir)
