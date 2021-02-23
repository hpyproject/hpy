.PHONY: all
all: hpy.universal

.PHONY: hpy.universal
hpy.universal:
	python3 setup.py build_ext -if

debug:
	HPY_DEBUG=1 make all

autogen:
	python3 -m hpy.tools.autogen .

cppcheck-build-dir:
	mkdir -p $(or ${CPPCHECK_BUILD_DIR}, .cppcheck)

cppcheck: cppcheck-build-dir
	# azure pipelines doesn't show stderr, so we write the errors to a file and cat it later :(
	cppcheck \
		--error-exitcode=1 \
		--cppcheck-build-dir=$(or ${CPPCHECK_BUILD_DIR}, .cppcheck) \
		--output-file=$(or ${CPPCHECK_BUILD_DIR}, .cppcheck)/output.txt \
		--enable=warning,performance,portability,information,missingInclude \
		--inline-suppr \
		--suppress=allocaCalled \
		-I hpy/devel/include/ \
		-I hpy/devel/include/common/ \
		-I hpy/devel/include/cpython/ \
		-I hpy/devel/include/universal/ \
		-I hpy/universal/src/ \
		--force \
		-D NULL=0 \
		. || (cat $(or ${CPPCHECK_BUILD_DIR}, .cppcheck)/output.txt && false)

infer:
	python3 setup.py build_ext -if -U NDEBUG | compiledb
	# see commit cd8cd6e for why we need to ignore debug_ctx.c
	@infer --fail-on-issue --compilation-database compile_commands.json --report-blacklist-path-regex "hpy/debug/src/debug_ctx.c"

valgrind:
	PYTHONMALLOC=malloc valgrind --suppressions=hpy/tools/valgrind/python.supp --suppressions=hpy/tools/valgrind/hpy.supp --leak-check=full --show-leak-kinds=definite,indirect --log-file=/tmp/valgrind-output python -m pytest --valgrind --valgrind-log=/tmp/valgrind-output test/

porting-example-tests:
	python docs/porting-example/setup.py install
	pytest docs/porting-example
