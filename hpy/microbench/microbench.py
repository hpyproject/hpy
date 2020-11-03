import time
from hpy.microbench import cpy_simple

N = 1000000

class BenchModule:

    def bench_noargs(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.noargs()

    def bench_onearg_None(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.onearg(None)

    def bench_onearg_int(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.onearg(i)

    def bench_varargs(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.varargs(None, None)

    def bench_allocate_int(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.allocate_int()

    def bench_allocate_tuple(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.allocate_tuple()
