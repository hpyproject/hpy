# To run the microbenchmarks

## Non-Python dependencies

The benchmarks depends on Valgrind, which can be installed with

```sh
sudo apt update && sudo apt install -y valgrind
```

Alternatively, you can also use [Pixi] to get it in a new shell with

```sh
pixi shell
```

Similarly, building with GraalPy requires `libffi` to build the Python package `ffi`.
`pixi shell` provides it.

[UV] can be useful since it is used in few Makefile targets to install Python interpreters.

## Python virtual environments

We assume in the following that a virtual environment is activated. One can create
environments with the Makefile targets `create_venv_...` as

```sh
cd /path/to/hpy/microbench
make create_venv_pypy
. .venv_pypy/bin/activate
```

## Non-editable install with build isolation

One can build these microbenchmarks with HPy from PyPI (on CPython) or bundled with the Python implementation.

```sh
pip install .
```

This builds the HPy extension with the CPython ABI for CPython and with the universal ABI for other implementations.
To build this extension with the universal ABI with CPython:

```sh
pip install . --config-settings="--global-option=--hpy-abi=universal"
```

## Editable install without build isolation

1. On CPython, you need to have `hpy` installed in your virtualenv. The easiest way
   to do it is:

   ```sh
   cd /path/to/hpy
   pip install -e .
   ```

2. Install build and runtime dependencies

   ```sh
   # cffi needed to build _valgrind
   pip install cffi pytest
   ```

3. Build and install the extension modules needed for the microbenchmarks

   ```sh
   cd /path/to/hpy/microbench
   pip install . --no-build-isolation
   # or for the universal ABI (on with CPython)
   rm -f src/*.so src/hpy_simple.py
   pip install -e . --no-build-isolation --config-settings="--global-option=--hpy-abi=universal"
   ```

## Run the benchmarks

```sh
pytest -v
```

To run only cpy or hpy tests, use -m (to select markers):

```sh
pytest -v -m hpy
pytest -v -m cpy
```

## Comparing alternative Python implementations to CPython

One can run things like

```sh
make cleanall
pixi shell
make create_venv_cpy
make create_venv_pypy
make create_venv_graalpy

make install PYTHON=.venv_cpy/bin/python
make install PYTHON=.venv_pypy/bin/python
make install PYTHON=.venv_graalpy/bin/python

make bench PYTHON=.venv_cpy/bin/python
make bench PYTHON=.venv_pypy/bin/python
# only HPy for GraalPy since the full benchmarks are a bit too long
make bench_hpy PYTHON=.venv_graalpy/bin/python

make print_cpy
make print_pypy
make print_graalpy

make print_pypy_vs_cpy
make print_graalpy_vs_cpy
```

[Pixi]: https://pixi.sh
[UV]: https://docs.astral.sh/uv/
