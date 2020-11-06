To run the microbenchmarks
--------------------------

1. You need to have `hpy.devel` installed in your virtuanenv. The easiest way
   to do it is:
   
       $ cd /path/to/hpy
       $ python setup.py develop

2. Build the extension modules needed for the microbenchmarks

       $ cd microbench
       $ pip install cffi # needed to build _valgrind
       $ python setup.py build_ext --inplace

2. `py.test -v`

3. To run only cpy or hpy tests, use -m (to select markers):

       $ py.test -v -m hpy
       $ py.test -v -m cpy
