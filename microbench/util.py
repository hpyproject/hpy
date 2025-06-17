import sys

from pathlib import Path


def get_short_prefix():
    prefix = Path(sys.prefix)
    name = prefix.name
    parts = [".", "venv", "_", "-"]
    for part in parts:
        if name.startswith(part):
            name = name[len(part) :]
    return name


def info_from_path(path):
    txt = path.read_text()
    _, txt = txt.split(
        "===================================== INFO ====================================="
    )
    txt, _ = txt.split(
        "================================== BENCHMARKS =================================="
    )
    result = {}
    for line in txt.splitlines():
        if not line:
            continue
        left, right = line.split(":")
        result[left] = right.strip()
    return result


def capitalize_implementation(implementation):
    return implementation.capitalize().replace("py", "Py")


def short_implementation(implementation):
    if implementation == "cpython":
        return "cpy"
    return implementation


def print_sep(out, sep_char="=", num_chars=81):
    out = f" {out} "
    num_sep_chars = (num_chars - len(out)) // 2
    print(num_sep_chars * "=" + out + num_sep_chars * "=")


def print_file_name_results():
    name = f"tmp_results_{get_short_prefix()}.txt"
    print(name)
