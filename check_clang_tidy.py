#!/usr/bin/env python3

import logging
import subprocess
import sys
from pathlib import Path

import _utils as utils


def check(build_dir, path):
    errors = None
    try:
        subprocess.check_output(["clang-tidy", "-quiet", "-p", build_dir, "-warnings-as-errors", "*", path],
                                stderr=subprocess.STDOUT, text=True)
    except subprocess.CalledProcessError as ex:
        errors = ex.output

    def result(progress):
        logging.info(f"[{progress}] checked clang-tidy: {path}")
        if not errors:
            return True
        logging.warning("errors found:")
        print(errors, file=sys.stderr)
        return False

    return result


if __name__ == "__main__":
    utils.init()

    build_dir = sys.argv[1] if len(sys.argv) > 1 else "build"
    if not (Path(build_dir) / "compile_commands.json").exists():
        logging.critical("compile_commands.json not found, did you pass the correct build directory ?")
        exit(2)

    utils.foreach_file(lambda path: check(build_dir, path),
                       lambda path: path.endswith(".cpp"))
