#!/usr/bin/env python3

import difflib
import logging
import subprocess
import sys

import _utils as utils


def check(path):
    with open(path) as f:
        original = f.read()
    formatted = subprocess.check_output(["clang-format", "-assume-filename", path],
                                        input=original, text=True)
    diff = difflib.unified_diff(original.splitlines(keepends=True),
                                formatted.splitlines(keepends=True),
                                path,
                                "formatted")
    diff_result = list(diff)

    def result(progress):
        logging.info(f"[{progress}] checked clang-format: {path}")
        if not diff_result:
            return True
        logging.warning("differences found:")
        sys.stderr.writelines(diff_result)
        print()
        return False

    return result


if __name__ == "__main__":
    utils.init()

    def filter(path):
        return (path.endswith(".cpp") or path.endswith(".h")) and not path.startswith("cmake/")
    utils.foreach_file(check, filter)
