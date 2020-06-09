all:
	python3 setup.py build_ext -if

debug:
	HPY_DEBUG=1 python3 setup.py build_ext -if

autogen:
	python3 tools/autogen.py
