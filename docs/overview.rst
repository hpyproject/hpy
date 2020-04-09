HPy overview
=============

Motivation and goals
---------------------

The biggest quality of the Python ecosystem is to have a huge number of high
quality libraries for all kind of jobs. Many of them, especially in the
scientific community, are written in C and exposed to Python using the
`Python/C API <https://docs.python.org/3/c-api/index.html>`_.  However, the
Python/C API exposes a number of CPython's implementation details and
low-level data structure layouts. This has two important consequences:

  1. any alternative implementation which want to support C extensions needs
     to either follow the same low-level layout or to provide a compatibility
     layer.

  2. CPython developers cannot experiment with new designs or refactoring
     without breaking compatibility with existing extensions.


During the course of the years, it became evident that emulating the Python/C
API in an efficient way is `challenging, if not impossible
<https://morepypy.blogspot.com/2018/09/inside-cpyext-why-emulating-cpython-c.html>`_.
The main goal of HPy is provide a **C API which is possible to implement in an
efficient way on a number of very diverse implementations**.  The following is
a list of sub-goals.

Performance on CPython
  HPy is usable on CPython from day 1 with no performance impact compared to
  the existing Python/C API.


Incremental adoption
  It is possible to port existing C extensions piece by piece and to use
  the old and the new API side-by-side during the transition.


Easy migration
  It should be easy to migrate existing C extensions to HPy. Thanks to an
  appropriate and regular naming convention is should be obvious what is the
  HPy equivalent of any existing Python/C API.  When a perfect replacement
  does not exist, the documentation explains what are the alternative options.


Better debugging
  In debug mode, you get early and precise errors and warnings when you make
  some specific kind of mistakes and/or violate the API rules and
  assumptions. For example, you get an error if you try to use a handle which
  has already been closed. It is possible to turn on the debug mode at startup
  time, *without needing to recompile*.


Hide internal details
  The API is designed to allow a lot of flexibility for Python
  implementations, allowing the possibility to explore different choices than
  the ones used by CPython.  In particular, **reference counting is not part
  of the API**: we want a more generic way of managing resources which is
  possible to implement with different strategies, including the existing
  reference counting and/or with a moving GC (like the ones used by PyPy or
  Java, for example).

  Moreover, we want to avoid exposing internal details of a specific
  implementation, so that each implementation can experiment with new memory
  layout of objects, add optimizations, etc.


Simplicity
  The HPy API aims to be smaller and easier to study/use/manage than the
  existing one. Sometimes there is a trade-off between this goal and the others
  above, in particular *Performance on CPython* and *Easy migration*.  The
  general approach is to have an API which is "as simple as possible" while
  not violating the other goals.


Universal binaries
  It is possible to compile extensions to a single binary which is
  ABI-compatible across multiple Python versions and/or multiple
  implementation. See :ref:`hpy-target-abis`.


Opt-in low level data structures
  Internal details might still be available, but in a opt-in way: for example,
  if Cython wants to iterate over a list of integers, it can ask if the
  implementation provides a direct low-level access to the content (e.g. in
  the form of a ``int64_t[]`` array) and use that. But at the same time, be
  ready to handle the generic fallback case.


API vs ABI
-----------

HPy defines *both* and API and an ABI. Before digging further into details,
let's distinguish them:

  - The **API** works at the level of source code: it is the set of functions,
    macros, types and structs which developers can use to write their own
    extension modules.  For C programs, the API is generally made available
    through one or more header files (``*.h``).

  - The **ABI** works at the level of compiled code: it is the interface between
    the host interpreter and the compiled DLL.  Given a target CPU and
    operating system it defines things like the set of exported symbols, the
    precise memory layout of objects, the size of types, etc.

In general it is possible to compile the same source into multiple compiled
libraries, each one targeting a different ABI. :pep:`3149` states that the
filename of the compiled extension should contain the *ABI tag* to specificy
what is the target ABI. For example, if you compile an extension called
``simple.c`` on CPython 3.7, you get a DLL called
``simple.cpython-37m-x86_64-linux-gnu.so``:

  - ``cpython-37m`` is the ABI tag, in this case CPython 3.7

  - ``x86_64`` is the CPU architecture

  - ``linux-gnu`` is the operating system

The same source code compiled on PyPy3.6 7.2.0 results in a file called
``simple.pypy3-72-x86_64-linux-gnu.so``:

  - ``pypy3-72`` is the ABI tag, in this case "PyPy3.x", version "7.2.x"

The HPy C API is exposed to the user by including ``hpy.h`` and it is
explained in its own section of the documentation.


.. _hpy-target-abis:

Target ABIs
----------------

Depending on the compilation options, and HPy extension can target three
different ABIs:

CPython
  In this mode, HPy is implemented as a set of C macros and ``static inline``
  functions which translate the HPy API into the CPython API at compile
  time. The result is a compiled extension which is indistinguishable from a
  "normal" one and can be distributed using all the standard tools and will
  run at the very same speed. The ABI tag is defined by the version of CPython
  which is used to compile it (e.g., ``cpython-37m``),

HPy Universal
  As the name suggests, the HPy Universal ABI is designed to be loaded and
  executed by a variety of different Python implementations. Compiled
  extensions can be loaded unmodified on all the interpreters which supports
  it.  PyPy supports it natively.  CPython supports it by using the
  ``hpy.universal`` package, and there is a small speed penalty compared to
  the CPython ABI.  The ABI tag has not been formally defined yet, but it will
  be something like ``hpy-1``, where ``1`` is the version of the API.

HPy Hybrid
  To allow an incremental transition to HPy, it is possible to use both HPy
  and Python/C API calls in the same extension. In this case, it is not
  possible to target the Universal ABI because the resulting compiled library
  also needs to be compatible with a specific CPython version. The ABI tag
  will be something like ``hpy-1_cpython-37m``.

Moreover, each alternative Python implementation could decide to implement its
own non-universal ABI if it makes sense for them. For example, a hypotetical
project *DummyPython* could decide to ship its own ``hpy.h`` which implements
the HPy API but generates a DLL which targets the DummyPython ABI.

This means that to compile an extension for CPython, you can choose whether to
target the CPython ABI or the Universal ABI. The advantage of the former is
that it runs at native speed, while the advantage of the latter is that you
can distribute a single binary, although with a small speed penalty on
CPython.  Obviously, nothing stops you to compile and distribute both
versions: this is very similar to what most projects are already doing, since
they automatically compile and distribute extensions for many different
CPython versions.


C extensions
--------------

If you are writing a Python extension in C, you are a consumer of the HPy
API. There are two big advantages in using HPy instead of the old Python/C
API:

  - Speed on PyPy and other alternative implementations: according to early
    :ref:`benchmarks`, an extension written in HPy can be ~3x faster than the
    equivalent extenson writting in Python/C.

  - Improved debugging: when you load extensions in :ref:`debugging mode`,
    many common mistakes are checked and reported automatically.

  - Universal binaries: you can choose to distribute only Universal ABI
    binaries. This comes with a small speed penalty con CPython, but for
    non-performance critical libraries it might still be a good tradeoff.


Cython extensions
-----------------

If you use Cython, you can't use HPy directly. The plan is to write a Cython
backend which emits HPy code instead of Python/C code: once this is done, you
will get the benefits of HPy automatically.


Extensions in other languages
------------------------------

On the API side, HPy is designed with C in mind, so it is not directly useful
if you want to write an extension in a language different than C.

However, Python bindings for other languages could decide to target the HPy
Universal ABI instead of the CPython ABI, and generate extensions which can be
loaded seamlessly on all Python implementations which supports it.  This is
the route taken for example by Rust (XXX put a link?).


Benefits for alternative Python implementations
------------------------------------------------

If you are writing an alternative Python implementation, there is a good
chance that you already know how painful it is to support the Python/C
API. HPy is designed to be both faster and easier to implement!

You have two choices:

  - support the Universal ABI: in this case, you just need to export the
    needed functions and to add a hook to ``dlopen()`` the desired libraries

  - use a custom ABI: in this case, you have to write your own replacement for
    ``hpy.h`` and recompile the C extensions with it.



Early benchmarks
-----------------

Projects involved
-----------------


Related work
-------------

A partial list of alternative implementations which offer a compatibility
layer include:

  - `PyPy <https://doc.pypy.org/en/latest/faq.html#do-cpython-extension-modules-work-with-pypy>`_

  - `Jython <https://www.jyni.org/>`_

  - `IronPython <https://github.com/IronLanguages/ironclad>`_

  - `GraalPython <https://github.com/graalvm/graalpython>`_
