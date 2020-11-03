import time
import cpy_simple

N = 1000000

class TestModule:

    def test_noargs(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.noargs()

    def test_onearg_None(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.onearg(None)

    def test_onearg_int(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.onearg(i)

    def test_varargs(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.varargs(None, None)

    def test_allocate_int(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.allocate_int()

    def test_allocate_tuple(self, timer):
        with timer:
            for i in range(N):
                cpy_simple.allocate_tuple()
