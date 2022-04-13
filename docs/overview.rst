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

  1. any alternative implementation which wants to support C extensions needs
     to either follow the same low-level layout or to provide a compatibility
     layer.

  2. CPython developers cannot experiment with new designs or refactoring
     without breaking compatibility with existing extensions.

Over the years, it has become evident that emulating the Python/C
API in an efficient way is `challenging, if not impossible
<https://www.pypy.org/posts/2018/09/inside-cpyext-why-emulating-cpython-c-8083064623681286567.html>`_.
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
  appropriate and regular naming convention it should be obvious what the
  HPy equivalent of any existing Python/C API is. When a perfect replacement
  does not exist, the documentation explains what the alternative options are.


Better debugging
  In debug mode, you get early and precise errors and warnings when you make
  some specific kind of mistakes and/or violate the API rules and
  assumptions. For example, you get an error if you try to use a handle
  (see :ref:`handles`) which has already been closed. It is possible to
  turn on the debug mode at startup time, *without needing to recompile*.


Hide internal details
  The API is designed to allow a lot of flexibility for Python
  implementations, allowing the possibility to explore different choices to
  the ones used by CPython.  In particular, **reference counting is not part
  of the API**: we want a more generic way of managing resources which is
  possible to implement with different strategies, including the existing
  reference counting and/or with a moving *Garbage Collector* (like the ones
  used by PyPy, GraalPython or Java, for example).

  Moreover, we want to avoid exposing internal details of a specific
  implementation, so that each implementation can experiment with new memory
  layout of objects, add optimizations, etc.


Simplicity
  The HPy API aims to be smaller and easier to study/use/manage than the
  existing Python/C API. Sometimes there is a trade-off between this goal and
  the others above, in particular *Performance on CPython* and *Easy migration*.
  The general approach is to have an API which is "as simple as possible" while
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

HPy defines *both* an API and an ABI. Before digging further into details,
let's distinguish them:

  - The **API** works at the level of source code: it is the set of functions,
    macros, types and structs which developers can use to write their own
    extension modules.  For C programs, the API is generally made available
    through one or more header files (``*.h``).

  - The **ABI** works at the level of compiled code: it is the interface between
    the host interpreter and the compiled DLL. Given a target CPU and
    operating system it defines things like the set of exported symbols, the
    precise memory layout of objects, the size of types, etc.

In general it is possible to compile the same source into multiple compiled
libraries, each one targeting a different ABI. :pep:`3149` states that the
filename of the compiled extension should contain the *ABI tag* to specify
what the target ABI is. For example, if you compile an extension called
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
-----------

Depending on the compilation options, and HPy extension can target three
different ABIs:

.. glossary::

    CPython ABI
      In this mode, HPy is implemented as a set of C macros and ``static inline``
      functions which translate the HPy API into the CPython API at compile
      time. The result is a compiled extension which is indistinguishable from a
      "normal" one and can be distributed using all the standard tools and will
      run at the very same speed. The ABI tag is defined by the version of CPython
      which is used to compile it (e.g., ``cpython-37m``),

    HPy Universal ABI
      As the name suggests, the HPy Universal ABI is designed to be loaded and
      executed by a variety of different Python implementations. Compiled
      extensions can be loaded unmodified on all the interpreters which supports
      it.  PyPy and GraalPython support it natively.  CPython supports it by using the
      ``hpy.universal`` package, and there is a small speed penalty compared to
      the CPython ABI.  The ABI tag has not been formally defined yet.

    HPy Hybrid ABI
      To allow an incremental transition to HPy, it is possible to use both HPy
      and Python/C API calls in the same extension. In this case, it is not
      possible to target the Universal ABI because the resulting compiled library
      also needs to be compatible with a specific CPython version. The ABI tag
      will be something like ``hpy-1_cpython-37m``. Note: the tag is not implemented
      yet. Currently, the approach to use HPy in hybrid mode is to build the extension
      in HPy universal mode, which, for now, still allows mixing the HPy and CPython APIs.
      Extensions mixing the HPy and CPython APIs will not work on Pythons that do not
      support the hybrid ABI.

Moreover, each alternative Python implementation could decide to implement its
own non-universal ABI if it makes sense for them. For example, a hypothetical
project *DummyPython* could decide to ship its own ``hpy.h`` which implements
the HPy API but generates a DLL which targets the DummyPython ABI.

This means that to compile an extension for CPython, you can choose whether to
target the CPython ABI or the Universal ABI. The advantage of the former is
that it runs at native speed, while the advantage of the latter is that you
can distribute a single binary, although with a small speed penalty on
CPython.  Obviously, nothing stops you compiling and distributing both
versions: this is very similar to what most projects are already doing, since
they automatically compile and distribute extensions for many different
CPython versions.

From the user point of view, extensions compiled for the CPython ABI can be
distributed and installed as usual, while those compiled for the HPy Universal
or HPy Hybrid ABIs require installing the ``hpy.universal`` package on
CPython and have no further requirements on Pythons that support HPy natively.


C extensions
------------

If you are writing a Python extension in C, you are a consumer of the HPy
API. There are three big advantages in using HPy instead of the old Python/C
API:

  - Speed on PyPy, GraalPython and other alternative implementations: according
    to early :ref:`benchmarks`, an extension written in HPy can be ~3x faster
    than the equivalent extension written in Python/C.

  - Improved debugging: when you load extensions in :ref:`debug mode`,
    many common mistakes are checked and reported automatically.

  - Universal binaries: you can choose to distribute only Universal ABI
    binaries. This comes with a small speed penalty on CPython, but for
    non-performance critical libraries it might still be a good tradeoff.


Cython extensions
-----------------

If you use Cython, you can't use HPy directly. There is a work in progress to
add Cython backend which emits HPy code instead of Python/C code: once this is
done, you will get the benefits of HPy automatically.


Extensions in other languages
-----------------------------

On the API side, HPy is designed with C in mind, so it is not directly useful
if you want to write an extension in a language other than C.

However, Python bindings for other languages could decide to target the
:term:`HPy Universal ABI` instead of the :term:`CPython ABI`, and generate
extensions which can be loaded seamlessly on all Python implementations which
supports it.  This is the route taken, for example, by `Rust
<https://github.com/pyhandle/rust-hpy>`_.


Benefits for alternative Python implementations
-----------------------------------------------

If you are writing an alternative Python implementation, there is a good
chance that you already know how painful it is to support the Python/C
API. HPy is designed to be both faster and easier to implement!

You have two choices:

  - support the Universal ABI: in this case, you just need to export the
    needed functions and to add a hook to ``dlopen()`` the desired libraries

  - use a custom ABI: in this case, you have to write your own replacement for
    ``hpy.h`` and recompile the C extensions with it.


Current status and roadmap
--------------------------

HPy is still in the early stages of development, but many big pieces are
already in place. As on April 2022, the following milestones have been reached:

  - few real-world Python packages have been ported to HPy API.
    The ports will be published soon.

  - one can write extensions which expose module-level functions, with all
    the various kinds of calling conventions.

  - there is support for argument parsing (i.e., the equivalents of
    `PyArg_ParseTuple` and `PyArg_ParseTupleAndKeywords`), and a
    convenient complex value building (i.e., the equivalent `Py_BuildValue`).

  - one can implement custom types, whose struct may contain references to other
    Python objects using `HPyField`.

  - there is a support for globally accessible Python object handles: `HPyGlobal`,
    which can still provide isolation for subinterpreters if needed.

  - there is support for raising and catching exceptions.

  - debug mode has been implemented and can be activated at run-time without
    recompiling. It can detect leaked handles or handles used after
    being closed.

  - wheels can be build for HPy extensions with `python setup.py bdist_wheel`
    and can be installed with `pip install`.

  - it is possible to choose between the :term:`CPython ABI` and the
    :term:`HPy Universal ABI` when compiling an extension module.

  - extensions compiled with the CPython ABI work out of the box on
    CPython.

  - it is possible to load HPy Universal extensions on CPython, thanks to the
    ``hpy.universal`` package.

  - it is possible to load HPy Universal extensions on
    PyPy (using the PyPy `hpy branch <https://foss.heptapod.net/pypy/pypy/tree/branch/hpy>`_).

  - it is possible to load HPy Universal extensions on `GraalPython
    <https://github.com/graalvm/graalpython>`_.


However, there is still a long road before HPy is usable for the general
public. In particular, the following features are on our roadmap but have not
been implemented yet:

  - many of the original Python/C functions have not been ported to
    HPy yet. Porting most of them is straightforward, so for now the priority
    is to test HPy with real-world Python packages and primarily resolve the
    "hard" features to prove that the HPy approach works.

  - add C-level module state to complement the `HPyGlobal` approach. While `HPyGlobal`
    is easier to use, it will make the migration simpler for existing extensions that
    use CPython module state.

  - the integration with Cython is work in progress

  - it is not clear yet how to approach pybind11 and similar C++ bindings. They serve two use-cases:

    - As C++ wrappers for CPython API. HPy is fundamentally different in some ways, so fully compatible
      pybind11 port of this API to HPy does not make sense. There can be a similar or even partially pybind11
      compatible C++ wrapper for HPy adhering to the HPy semantics and conventions (e.g., passing the
      HPyContext pointer argument around, no reference stealing, etc.).

    - Way to expose (or "bind") mostly pure C++ functions as Python functions where the C++ templating
      machinery takes care of the conversion between the Python world, i.e., ``PyObject*``, and the C++
      types. Porting this abstraction to HPy is possible and desired in the future. To determine the priority
      or such effort, we need to get more knowledge about existing pybind11 use-cases.


.. _benchmarks:

Early benchmarks
-----------------

To validate our approach, we ported a simple yet performance critical module
to HPy. We chose `ultrajson <https://github.com/pyhandle/ultrajson-hpy>`_
because it is simple enough to require porting only a handful of API
functions, but at the same time it is performance critical and performs many
API calls during the parsing of a JSON file.

This `blog post <https://www.pypy.org/posts/2019/12/hpy-kick-off-sprint-report-1840829336092490938.html>`_
explains the results in more detail, but they can be summarized as follows:

  - ``ujson-hpy`` compiled with the CPython ABI is as fast as the original
    ``ujson``.

  - A bit surprisingly, ``ujson-hpy`` compiled with the HPy Universal ABI is
    only 10% slower on CPython.  We need more evidence than a single benchmark
    of course, but if the overhead of the HPy Universal ABI is only 10% on
    CPython, many projects may find it small enough that the benefits
    of distributing extensions using only the HPy Universal ABI out weight
    the performance costs.

  - On PyPy, ``ujson-hpy`` runs 3x faster than the original ``ujson``. Note
    the HPy implementation on PyPy is not fully optimized yet, so we expect
    even bigger speedups eventually.


Projects involved
-----------------

HPy was born during EuroPython 2019, were a small group of people started to
discuss the problems of the Python/C API and how it would be nice to
have a way to fix them.  Since then, it has gathered the attention and interest
of people who are involved in many projects within the Python ecosystem.  The
following is a (probably incomplete) list of projects whose core developers
are involved in HPy, in one way or the other.  The mere presence in this list
does not mean that the project as a whole endorse or recognize HPy in any way,
just that some of the people involved contributed to the
code/design/discussions of HPy:

  - PyPy

  - CPython

  - Cython

  - GraalPython

  - RustPython

  - rust-hpy (fork of the `cpython crate <https://crates.io/crates/cpython>`_)




Related work
-------------

A partial list of alternative implementations which offer a Python/C
compatibility layer include:

  - `PyPy <https://doc.pypy.org/en/latest/faq.html#do-cpython-extension-modules-work-with-pypy>`_

  - `Jython <https://www.jyni.org/>`_

  - `IronPython <https://github.com/IronLanguages/ironclad>`_

  - `GraalPython <https://github.com/graalvm/graalpython>`_
