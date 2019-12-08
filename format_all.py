#!/usr/bin/env python3

import logging
import subprocess

import _utils as utils


def format(path):
    subprocess.check_call(["clang-format", "-i", path])

    def result(progress):
        logging.info(f"[{progress}] formatted: {path}")
        return True

    return result


if __name__ == "__main__":
    utils.init()

    def filter(path):
        return (path.endswith(".cpp") or path.endswith(".h")) and not path.startswith("cmake/")
    utils.foreach_file(format, filter)
