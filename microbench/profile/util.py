import sys

if "purepy" in sys.argv:
    import purepy_simple as simple
elif "cpy" in sys.argv:
    import cpy_simple as simple
else:
    if sys.implementation.name == "cpython":
        import cpy_simple as simple
    else:
        import hpy_simple as simple

print(simple)

cls = simple.HTFoo

N = 10000000
if sys.implementation.name == "cpython":
    N *= 10
elif sys.implementation.name == "pypy" and "cpy" in sys.argv:
    N *= 4

if "--short" in sys.argv:
    N //= 100
