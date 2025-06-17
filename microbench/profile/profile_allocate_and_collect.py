import gc

from time import perf_counter as time

from util import cls, N


def main():
    objs = [None] * N
    for i in range(N):
        objs[i] = cls()
    return objs


gc.collect()

t_start = time()
_objs = main()
del _objs
gc.collect()
print(f"time per object: {(time() - t_start)/N:.1e} s")
