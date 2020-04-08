HPy overview
=============

Motivation and goals
---------------------

The biggest quality of the Python ecosystem is to have a huge number of high
quality libraries for all kind of jobs. Many of them, especially in the
scientific community, are written in C and exposed to Python using the
Python/C API.

However, the Python/C API exposes a number of CPython's implementation details
and low-level data structure layout. This has two important consequences:

  1. any alternative implementation which want to support C extensions needs
     to either follow the same low-level layout or to provide a compatibility
     layer.

  2. CPython developers cannot experiment with new designs or refactoring
     without breaking compatibility with existing extensions.


During the course of the years, it became evident that emulating the Python/C
API in an efficient way is `challenging, if not impossible
<https://morepypy.blogspot.com/2018/09/inside-cpyext-why-emulating-cpython-c.html>`_. A
partial list of alternative implementations which offer a compatibility layer
include:

  - `PyPy <https://doc.pypy.org/en/latest/faq.html#do-cpython-extension-modules-work-with-pypy>`_

  - `Jython <https://www.jyni.org/>`_

  - `IronPython <https://github.com/IronLanguages/ironclad>`_

  - `GraalPython <https://github.com/graalvm/graalpython>`_

The main goal of HPy is provide a C API which is possible to implement in an
efficient way on a number of very diverse implementations.  The other
sub-goals include (but are not necessarily limited to):

  - to be usable on CPython *right now* with no (or almost no) performance
    impact

  - to make the adoption **incremental**: it should be possible to migrate
    existing C extensions piece by piece and to use the old and the new API
    side-by-side during the transition

  - to provide better debugging experience: in debug mode, you could get
    precise notification about which handles are kept open for too long
    or used after being closed.

  - to be more friendly for other implementations: in particular, we do not
    want reference counting to be part of the API: we want a more generic way
    of managing resources which is possible to implement with different
    strategies, including the existing reference counting and/or with a moving
    GC (like the ones used by PyPy or Java, for example)

  - to be smaller and easier to study/use/manage than the existing one

  - to avoid to expose internal details of a specific implementation, so that
    each implementation can experiment with new memory layout of objects, add
    optimizations, etc.

  - to be written in a way which could make it possible in the future to have
    a single binary which is ABI-compatible across multiple Python versions
    and/or multiple implementations

  - internal details might still be available, but in a opt-in way: for
    example, if Cython wants to iterate over a list of integers, it can ask if
    the implementation provides a direct low-level access to the content
    (e.g. in the form of a ``int64_t[]`` array) and use that. But at the same
    time, be ready to handle the generic fallback case.


API vs ABI
-----------

HPy defines *both* and API and an ABI. Before digging further into details,
let's distinguish them:

  - The API works at the level of source code: it is the set of functions,
    macros, types and structs which developers can use to write their own
    extension modules.  For C programs, the API is generally made available
    throuh one or more header file (``*.h``).

  - The ABI works at the level of compiled code: it is the interface between
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

  - ``pypy3-72`` mean "PyPy3.x", version "7.2.x"

The HPy C API is exposed to the user by including ``hpy.h`` and it is
explained in its own section of the documentation.


HPy target ABIs
----------------

Depending on the compilation options, and HPy extension can target three
different ABIs:

CPython
  In this mode, HPy is implemented as a set of C macros and ``static inline``
  functions which translate the HPy API into the CPython API at compile
  time. The result is a compiled extension which is indistinguishable from a
  "normal" one and can be distributed using all the standard tools. The ABI
  tag is defined by the version of CPython which is used to compile it (e.g.,
  ``cpython-37m``).

Universal
  As the name suggests, the HPy Universal ABI is designed to be loaded and
  executed by a variety of different Python implementations. Compiled
  extensions can be loaded unmodified on all the interpreters which supports
  it.  PyPy supports it natively.  CPython supports it by using the
  ``hpy.universal`` package.  The ABI tag has not been formally defined yet,
  but it will be something like ``hpy-1``, where ``1`` is the version of the
  API.

Hybrid
  To allow an incremental transition to HPy, it is possible to use both HPy
  and Python/C API calls in the same extension. In this case, it is not
  possible to target the Universal ABI because the resulting compiled library
  also needs to be compatible with a specific CPython version.. The ABI tag
  will be something like ``hpy-1_cpython-37m``.



Extension writers vs implementation writers
--------------------------------------------

Projects involved
-----------------
