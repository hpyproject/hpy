all:
	python3 setup.py build_ext -if

debug:
	HPY_DEBUG=1 python3 setup.py build_ext -if

autogen:
	python3 -m hpy.tools.autogen .

cppcheck-build-dir:
	mkdir -p $(or ${CPPCHECK_BUILD_DIR}, .cppcheck)

cppcheck: cppcheck-build-dir
	cppcheck --error-exitcode=1 --cppcheck-build-dir=$(or ${CPPCHECK_BUILD_DIR}, .cppcheck) --enable=warning,performance,portability,information,unusedFunction,missingInclude --report-progress -I hpy/devel/include/ -I hpy/devel/include/common/ -I hpy/devel/include/cpython/ -I hpy/devel/include/universal/ -I hpy/universal/src/ --suppress=nullPointerRedundantCheck --force -D NULL=0 .

infer:
	python3 setup.py build_ext -if -U NDEBUG | compiledb
	@infer --fail-on-bug --compilation-database compile_commands.json

valgrind:
	PYTHONMALLOC=malloc valgrind --suppressions=hpy/tools/valgrind/python.supp --suppressions=hpy/tools/valgrind/hpy.supp --leak-check=full --show-leak-kinds=definite,indirect --log-file=/tmp/valgrind-output python -m pytest --valgrind --valgrind-log=/tmp/valgrind-output test/
