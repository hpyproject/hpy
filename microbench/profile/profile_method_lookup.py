import gc

from time import perf_counter as time

from util import cls, N

N *= 40

def main():
    obj = cls()
    for _ in range(N):
        # note: here we are NOT calling it, we want to measure just
        # the lookup
        obj.noargs  # pylint: disable=W0104


gc.collect()

t_start = time()
main()
print(f"time per lookup: {(time() - t_start)/N:.1e} s")
