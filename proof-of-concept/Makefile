pof.cpython-37m-x86_64-linux-gnu.so: pof.c
	python3.7 setup.py build_ext -i -f

.PHONY: test
test: pof.cpython-37m-x86_64-linux-gnu.so
	python3.7 -m pytest test_pof.py

universal: pof.hpy.so

pof.hpy.so: pof.c
	gcc -shared -fPIC -o pof.hpy.so -I../hpy/devel/include -DHPY_UNIVERSAL_ABI pof.c
