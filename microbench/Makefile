ifeq ($(PYTHON),)
PYTHON := python3
endif

install:
	$(PYTHON) -m pip install .

install_universal:
	$(PYTHON) -m pip install . --config-settings="--global-option=--hpy-abi=universal"

uninstall:
	$(PYTHON) -m pip uninstall hpy.microbench --yes

test:
	$(PYTHON) -m pytest -v --purepy | tee $(shell $(PYTHON) -c 'import util as u; u.print_file_name_results()')

bench: test

bench_hpy:
	$(PYTHON) -m pytest -v -m hpy | tee $(shell $(PYTHON) get_file_name_results.py)

clean:
	rm -f src/*.so src/hpy_simple.py

cleanall: clean
	rm -rf .venv_* tmp_*.txt

create_venv_cpy:
	uv python install 3.12
	$(shell uv python find 3.12) -m venv .venv_cpy --upgrade-deps

create_venv_pypy:
	uv python install pypy
	$(shell uv python find pypy) -m venv .venv_pypy --upgrade-deps

create_venv_graalpy:
	uv python install graalpy
	# cannot use --upgrade-deps because pip is patched for GraalPy
	$(shell uv python find graalpy) -m venv .venv_graalpy

print_cpy:
	@$(PYTHON) print_tmp_results.py cpy

print_pypy:
	@$(PYTHON) print_tmp_results.py pypy

print_graalpy:
	@$(PYTHON) print_tmp_results.py graalpy

print_pypy_vs_cpy:
	@$(PYTHON) print_other_vs_cpy.py pypy

print_graalpy_vs_cpy:
	@$(PYTHON) print_other_vs_cpy.py graalpy
