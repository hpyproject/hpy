"""
These are not real tests, but microbenchmarks. The machinery to record the
timing and display the results is inside conftest.py
"""

import pytest
import time

API_PARAMS = [
    pytest.param('cpy', marks=pytest.mark.cpy),
    pytest.param('hpy', marks=pytest.mark.hpy)
    ]

@pytest.fixture(params=API_PARAMS)
def api(request):
    return request.param

@pytest.fixture
def simple(request, api):
    if api == 'cpy':
        import cpy_simple
        return cpy_simple
    elif api == 'hpy':
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

    def test_allocate_tuple(self, api, simple, timer):
        if api == 'hpy':
            pytest.skip('Missing HPy_BuildValue')
        with timer:
            for i in range(N):
                simple.allocate_tuple()


class TestType:

    def test_noargs(self, simple, timer):
        obj = simple.Foo()
        with timer:
            for i in range(N):
                obj.noargs()

    def test_onearg_None(self, simple, timer):
        obj = simple.Foo()
        with timer:
            for i in range(N):
                obj.onearg(None)

    def test_onearg_int(self, simple, timer):
        obj = simple.Foo()
        with timer:
            for i in range(N):
                obj.onearg(i)

    def test_varargs(self, simple, timer):
        obj = simple.Foo()
        with timer:
            for i in range(N):
                obj.varargs(None, None)

    def test_len(self, simple, timer):
        obj = simple.Foo()
        with timer:
            for i in range(N):
                len(obj)

    def test_getitem(self, simple, timer):
        obj = simple.Foo()
        with timer:
            for i in range(N):
                obj[0]
