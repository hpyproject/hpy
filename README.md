PyHandle: a better API for Python
==================================

The goal of the project is to design a better API for extending Python
in C. The current API is specific to the current implementation of CPython: it
exposes a lot of internal details which makes it hard:

  - to implement it for other Python implementations (e.g. PyPy, IronPython, etc.)

  - to experiment with new things inside CPython itself: e.g. using a GC
    instead of refcounting, or to remove the GIL

The goal of this project is to improve the situation by designing a new API
which solves some of the current problems.

More specifically, the goals include (but are not necessarily limited to):

  - to be usable on CPython *right now* with no (or almost no) performance
    impact

  - to make the adoption **incremental**: it should be possible to migrate
    existing C extensions piece by piece and to use the old and the new API
    side-by-side during the transition

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


More concrete goals
--------------------

  - we will write a small C library which implements the new API on top of the
    existing one: no changes to CPython needed

  - PyPy will implement this natively: extensions using this API will be
    orders of magnitude faster than the ones using the existing old API (see
    [this blog post](https://morepypy.blogspot.com/2018/09/inside-cpyext-why-emulating-cpython-c.html)
    for details)

  - Cython will adopt this from day one: exiting Cython programs will benefit
    from this automatically


Why should I care about this stuff?
------------------------------------

  - the existing C API is becoming a problem for CPython and for the
    evolution of the language itself: this project makes it possible to make
    experiments which might be "officially" adopted in the future

  - for PyPy, it will give obvious speed benefits: for example, data
    scientists will be able to get the benefit of fast C libraries *and* fast
    Python code at the same time, something which is hard to achieve now

  - arguably, it will be easier to learn and understand than the massive
    CPython C API
