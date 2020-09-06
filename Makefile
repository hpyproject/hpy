all:
	python3 setup.py build_ext -if

debug:
	HPY_DEBUG=1 python3 setup.py build_ext -if

autogen:
	python3 -m hpy.tools.autogen .

cppcheck:
	PYTHON_INCLUDE_DIR=$(python $$PWD/hpy/tools/include_path.py)
	@cppcheck --error-exitcode=1 --enable=all --report-progress -I hpy/devel/include/ -I hpy/devel/include/common/ -I hpy/devel/include/cpython/ -I hpy/devel/include/universal/ -I hpy/universal/src/ -I $PYTHON_INCLUDE_DIR --force -D NULL=0 .
