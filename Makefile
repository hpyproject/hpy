all:
	python3 setup.py build_ext -if

autogen:
	python3 tools/autogen.py
