ifeq ($(PYTHON),)
PYTHON := python3
endif

install:
	$(PYTHON) -m pip install .

install-univ:
	$(PYTHON) -m pip install . --config-settings="--global-option=--hpy-abi=universal"

uninstall:
	$(PYTHON) -m pip uninstall hpy.microbench

test:
	$(PYTHON) -m pytest -v | tee tmp_results_$(shell $(PYTHON) -c "import sys; print(sys.implementation.name)").txt

clean:
	rm -f src/*.so src/hpy_simple.py

create_venv_cpy:
	$(shell uv python find 3.12) -m venv .venv_cpy --upgrade-deps

create_venv_pypy:
	$(shell uv python find pypy) -m venv .venv_pypy --upgrade-deps

create_venv_graalpy:
	# cannot use --upgrade-deps because pip is patched for GraalPy
	$(shell uv python find graalpy) -m venv .venv_graalpy

print_other_vs_cpy:
	@$(PYTHON) print_other_vs_cpy.py

print_cpy:
	@tail tmp_results_cpython.txt -n 30

print_pypy:
	@tail tmp_results_pypy.txt -n 30
