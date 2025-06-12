
import gc
import sys

from time import perf_counter as time

if "purepy" in sys.argv:
    import purepy_simple as simple
elif "cpy" in sys.argv:
    import cpy_simple as simple
else:
    if sys.implementation.name == "cpython":
        import cpy_simple as simple
    else:
        import hpy_simple as simple

print(simple)

cls = simple.HTFoo
N = 10000000
if sys.implementation.name == "cpython":
    N *= 10

def main():
    objs = [None] * N
    for i in range(N):
        objs[i] = cls()
    return objs

gc.collect()

t_start = time()
main()
print(f"time per allocation: {(time() - t_start)/N:.1e} s")
