pof.cpython-37m-x86_64-linux-gnu.so: pof.c
	python3 setup.py build_ext -i -f

pofcpp.cpython-37m-x86_64-linux-gnu.so: pofcpp.cpp
	python3 setup.py build_ext -i -f

.PHONY: test testcpp
test: pof.cpython-37m-x86_64-linux-gnu.so pofcpp.cpython-37m-x86_64-linux-gnu.so
	python3 -m pytest test_pof.py

universal: pof.hpy.so pofcpp.hpy.so

pof.hpy.so: pof.c
	gcc -shared -fPIC -o pof.hpy.so -I../hpy/devel/include -DHPY_UNIVERSAL_ABI pof.c

pofcpp.hpy.so: pofcpp.cpp
	gcc -shared -fPIC -o pofcpp.hpy.so -I../hpy/devel/include -DHPY_UNIVERSAL_ABI pofcpp.cpp
