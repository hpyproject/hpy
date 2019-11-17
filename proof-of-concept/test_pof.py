import pof

def test_do_nothing():
    assert pof.do_nothing() is None

def test_double():
    assert pof.double(21) == 42

def test_add_ints():
    assert pof.add_ints(30, 12) == 42
