import pof
import pofpackage.foo


def test_do_nothing():
    assert pof.do_nothing() is None

def test_double():
    assert pof.double(21) == 42

def test_add_ints():
    assert pof.add_ints(30, 12) == 42

def test_add_ints_kw():
    assert pof.add_ints_kw(b=30, a=12) == 42

def test_point():
    p = pof.Point(1, 2)
    assert repr(p) == 'Point(?, ?)' # fixme when we have HPyUnicode_FromFormat

def test_point_sum():
    p = pof.Point(23, 17)
    assert p.sum() == 40

def test_point_sum():
    p = pof.Point(1, 2)
    p.multiply(4, 5)
    assert p.sum() == 14

def test_point_compare_with_object():
    import pytest
    p = pof.Point(1, 2)
    o = object()
    with pytest.raises(TypeError) as exc_info:
        assert p == o
    assert exc_info.match("instance must be Point")

def test_point_compare_lt_not_implemented():
    import pytest
    p = pof.Point(1, 2)
    q = pof.Point(1, 2)
    with pytest.raises(TypeError) as exc_info:
        assert p > q
    assert exc_info.match("'>' not supported between instances of 'pof.Point' and 'pof.Point'")

def test_point_compare_eq():
    import pytest
    p = pof.Point(1, 2)
    q = pof.Point(1, 3)
    r = pof.Point(1, 3)

    assert not p == q
    assert q == r

def test_pofpackage():
    assert pofpackage.foo.__name__ == "pofpackage.foo"
    assert pofpackage.foo.hello() == 'hello from pofpackage.foo'
