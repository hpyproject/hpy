#!/usr/bin/env python3
import sys

from pathlib import Path

import util

try:
    arg = sys.argv[1]
except IndexError:
    arg = "pypy"

path_result = Path(arg)

if not path_result.exists():
    path_result = Path(f"tmp_results_{arg}.txt")

assert path_result.exists()

info = util.info_from_path(path_result)
implementation = info["implementation"]

util.print_sep(util.capitalize_implementation(implementation))
if info["short prefix"] != implementation and info[
    "short prefix"
] != util.short_implementation(implementation):
    print(f"short prefix: {info['short prefix']}")

txt = path_result.read_text()
_, txt = txt.split(
    "================================== BENCHMARKS ==================================\n"
)

print(txt.rstrip())
