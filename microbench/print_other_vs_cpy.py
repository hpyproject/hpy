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


def data_from_path(path, index_time):
    txt = path.read_text()
    _, txt = txt.split(
        "================================== BENCHMARKS =================================="
    )
    lines = txt.splitlines()[3:-2]

    names = []
    times = []

    for line in lines:
        parts = line.split()
        names.append(parts[0])
        times.append(float(parts[index_time]))

    return names, times


names, times_cpy = data_from_path(path_result_cpy, 1)
names, times_other = data_from_path(path_result_other, 3)

max_length_name = 45
fmt_name = f"{{:{max_length_name}s}}"

print(f"# {other} HPy univ / CPy native (time ratio, smaller is better)")

for index, t_other in enumerate(times_other):
    ratio = t_other / times_cpy[index]
    name = fmt_name.format(names[index])
    print(f"{name} {ratio:.2f}")
