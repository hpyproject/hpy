# Profile

## Disable the JIT

```sh
python --jit off profile_alloc.py
```

## With vmprof

```sh
pip install vmprof
sudo apt install libunwind-dev
python -m vmprof -o tmp_output.log profile_alloc.py
vmprofshow tmp_output.log tree
```

## with vmprof-firefox-converter

```sh
pip install vmprof-firefox-converter
vmprofconvert -run profile_alloc.py cpy
# or
vmprofconvert -run profile_alloc.py
```

## With `PYPYLOG`

```sh
PYPYLOG=jit-log-opt,jit-backend:logfile python profile_alloc_and_die.py
```

## With Valgrind / Callgrind

```sh
valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes python profile_alloc_and_die.py --short
```

The Valgrind output (a file starting with "callgrind.out.") can for example be
opened with `qcachegrind`.
