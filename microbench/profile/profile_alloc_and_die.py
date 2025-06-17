import gc

from time import perf_counter as time

from util import cls, N


def main():
    for _ in range(N):
        obj = cls()
        obj.onearg(None)


gc.collect()

t_start = time()
main()
print(f"time per allocation: {(time() - t_start)/N:.1e} s")
