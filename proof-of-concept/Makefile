pof.cpython-37m-x86_64-linux-gnu.so: pof.c
	python3.7 setup.py build_ext -i -f

.PHONY: test
test: pof.cpython-37m-x86_64-linux-gnu.so
	python3.7 -m pytest test_pof.py
