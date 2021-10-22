import simple
import mixed
import hpyvarargs


def test_simple_abs():
    assert simple.myabs(-42) == 42
    assert simple.myabs(42) == 42


def test_hpyvarargs():
    assert hpyvarargs.add_ints(40, 2) == 42


def test_mixed_add_ints():
    assert mixed.add_ints_legacy(40, 2) == 42
    assert mixed.add_ints(40, 2) == 42