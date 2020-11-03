"""
These are not real tests, but microbenchmarks. The machinery to record the
timing and display the results is inside conftest.py
"""

import pytest
import time

@pytest.fixture(params=['cpy', 'hpy'])
def simple(request):
    if request.param == 'cpy':
        import cpy_simple
        return cpy_simple
    elif request.param == 'hpy':
        import hpy_simple
        return hpy_simple
    else:
        assert False, 'Unkown param: %s' % request.param

N = 10000000

class TestModule:

    def test_noargs(self, simple, timer):
        with timer:
            for i in range(N):
                simple.noargs()

    def test_onearg_None(self, simple, timer):
        with timer:
            for i in range(N):
                simple.onearg(None)

    def test_onearg_int(self, simple, timer):
        with timer:
            for i in range(N):
                simple.onearg(i)

    def test_varargs(self, simple, timer):
        with timer:
            for i in range(N):
                simple.varargs(None, None)

    def test_allocate_int(self, simple, timer):
        with timer:
            for i in range(N):
                simple.allocate_int()

    def test_allocate_tuple(self, simple, timer):
        with timer:
            for i in range(N):
                simple.allocate_tuple()
