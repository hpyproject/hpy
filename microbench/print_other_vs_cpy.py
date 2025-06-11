import sys

from pathlib import Path

try:
    other = sys.argv[1]
except IndexError:
    other = "PyPy"

path_result_cpy = Path("tmp_results_cpython.txt")
path_result_other = Path(f"tmp_results_{other.lower()}.txt")

assert path_result_cpy.exists()
assert path_result_other.exists()


def data_from_path(path):
    txt = path.read_text()
    _, txt = txt.split(
        "================================== BENCHMARKS =================================="
    )
    lines = txt.splitlines()[3:-2]

    index_time_cpy = 1
    index_time_hpy = 3

    parts = lines[0].split()
    if len(parts) == 1:
        if "cpy" in path.name:
            index_time_hpy = None
        else:
            index_time_cpy = None
            index_time_hpy = 1

    names = []
    times_cpy = []
    times_hpy = []

    for line in lines:
        parts = line.split()
        names.append(parts[0])
        if index_time_cpy is not None:
            times_cpy.append(float(parts[index_time_cpy]))
        if index_time_hpy is not None:
            times_hpy.append(float(parts[index_time_hpy]))

    return names, times_cpy, times_hpy


names, times_cpy_cpy, times_cpy_hpy = data_from_path(path_result_cpy)
names, times_other_cpyext, times_other_hpy = data_from_path(path_result_other)

max_length_name = 45
fmt_name = f"{{:{max_length_name}s}}"

out = f" {other} / CPy native (time ratio, smaller is better) "
num_chars = 81
num_equals = (num_chars - len(out)) // 2

print("\n" + num_equals * "=" + out + num_equals * "=")

if times_other_cpyext:
    print(max_length_name * " " + "cpyext    HPy univ")
else:
    print(max_length_name * " " + "HPy univ")

for index, t_other_hpy in enumerate(times_other_hpy):
    norm = times_cpy_cpy[index]
    name = fmt_name.format(names[index])

    ratio = t_other_hpy / norm

    if times_other_cpyext:
        ratio_cpyext = times_other_cpyext[index] / norm
        print(f"{name} {ratio_cpyext:5.2f}      {ratio:5.2f}")
    else:
        print(f"{name} {ratio:.2f}")
