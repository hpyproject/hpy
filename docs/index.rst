.. HPy documentation master file, created by
   sphinx-quickstart on Thu Apr  2 23:01:08 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

HPy: a better API for Python
===============================

HPy provides a new API for extending Python in C.

The official `Python/C API <https://docs.python.org/3/c-api/index.html>`_ is
specific to the current implementation of CPython: it exposes a lot of
internal details which makes it hard:

  - to implement it for other Python implementations (e.g. PyPy, GraalPython,
    Jython, IronPython, etc.)

  - to experiment with new things inside CPython itself: e.g. using a GC
    instead of refcounting, or to remove the GIL.

There are several advantages to write your C extension in HPy:

  - it runs much faster on PyPy, and at native speed on CPython

  - it is possible to compile a single binary which runs unmodified on all
    supported Python implementations and versions

  - it is simpler and more manageable than the Python/C API

  - it provides an improved debugging experience: in "debug mode", HPy
    actively checks for many common mistakes such as reference leaks and
    invalide usage of objects after they have been deleted. It is possible to
    turn the "debug mode" on at startup time, without needing to recompile the
    CPython nor the extension itself


.. toctree::
   :maxdepth: 2

   overview


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
