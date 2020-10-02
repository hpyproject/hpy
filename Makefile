all:
	python3 setup.py build_ext -if

debug:
	HPY_DEBUG=1 python3 setup.py build_ext -if

autogen:
	python3 -m hpy.tools.autogen .

PYTHON_INCLUDE_DIR=$(shell python $$PWD/hpy/tools/include_path.py)

cppcheck-build-dir:
	mkdir -p $(or ${CPPCHECK_BUILD_DIR}, .cppcheck)

cppcheck: cppcheck-build-dir
	@cppcheck --error-exitcode=1 --cppcheck-build-dir=$(or ${CPPCHECK_BUILD_DIR}, .cppcheck) --enable=warning,performance,portability,information,unusedFunction,missingInclude --report-progress -I hpy/devel/include/ -I hpy/devel/include/common/ -I hpy/devel/include/cpython/ -I hpy/devel/include/universal/ -I hpy/universal/src/ -I ${PYTHON_INCLUDE_DIR} --force -D NULL=0 .

infer:
	python3 setup.py build_ext -if -U NDEBUG | compiledb
	@infer --fail-on-bug --compilation-database compile_commands.json
