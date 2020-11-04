To run the microbenchmarks:

1. python setup.py build_ext --inplace

2. py.test -v

3. To run only cpy or hpy tests, use -m (to select markers):
   py.test -v -m hpy
   py.test -v -m cpy
