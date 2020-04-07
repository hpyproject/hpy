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

  - `Jython <https://www.jyni.org/>`

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
    (e.g. in the form of a `int64_t[]` array) and use that. But at the same
    time, be ready to handle the generic fallback case.




API vs ABI
-----------

Extension writers vs implementation writers
--------------------------------------------

Projects involved
-----------------
