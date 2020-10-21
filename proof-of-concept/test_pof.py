import os

def import_universal_pof():
    # so far, we need to manually load&import universal modules. Eventually,
    # this should be supported directly by hpy, but it's still unclear how.
    import pathlib
    import hpy.universal
    root = pathlib.Path(__file__).parent
    class PofSpec:
        name = 'pof'
        origin = str(root.joinpath('pof.hpy.so'))
    class FooSpec:
        name = 'pofpackage.foo'
        origin = str(root.joinpath('pofpackage', 'foo.hpy.so'))

    import pofpackage
    pof = hpy.universal.load_from_spec(PofSpec)
    pofpackage.foo = hpy.universal.load_from_spec(FooSpec)
    return pof, pofpackage


if os.getenv('HPY_UNIVERSAL') == '1':
    pof, pofpackage = import_universal_pof()
else:
    # import pof modules as normal
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
    assert repr(p) == 'Point(?, ?)' # fixme when we have HPyFloat_FromDouble

def test_pofpackage():
    assert pofpackage.foo.hello() == 'hello from pofpackage.foo'
