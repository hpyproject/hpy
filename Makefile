all:
	python3 setup.py build_ext -if

debug:
	HPY_DEBUG=1 python3 setup.py build_ext -if

autogen:
	python3 -m hpy.tools.autogen .

valgrind:
	PYTHONMALLOC=malloc valgrind --suppressions=hpy/tools/valgrind/python.supp --suppressions=hpy/tools/valgrind/hpy.supp --leak-check=full --show-leak-kinds=definite,indirect --log-file=/tmp/valgrind-output python -m pytest --valgrind --valgrind-log=/tmp/valgrind-output test/
