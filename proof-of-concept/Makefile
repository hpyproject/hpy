pof.cpython-37m-x86_64-linux-gnu.so: pof.c hpy.h
	@touch pof.c # force build_ext to recompile if we touched only hpy.h
	python3.7 setup.py build_ext -i

.PHONY: test
test: pof.cpython-37m-x86_64-linux-gnu.so
	python3.7 -m pytest test_pof.py
