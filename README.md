# HPy: a better API for Python

[![Build](https://github.com/hpyproject/hpy/actions/workflows/ci.yml/badge.svg)](https://github.com/hpyproject/hpy/actions/workflows/ci.yml)
[![Documentation](https://readthedocs.org/projects/hpy/badge/)](https://hpy.readthedocs.io/)
[![Join the discord server at https://discord.gg/xSzxUbPkTQ](https://img.shields.io/discord/1077164940906995813.svg?color=7389D8&labelColor=6A7EC2&logo=discord&logoColor=ffffff&style=flat-square)](https://discord.gg/xSzxUbPkTQ)

**Website**: [hpyproject.org](https://hpyproject.org/) \
**Community**: [HPy Discord server](https://discord.gg/xSzxUbPkTQ) \
**Mailing list**: [hpy-dev@python.org](https://mail.python.org/mailman3/lists/hpy-dev.python.org/)

## Project status

HPy is no longer actively maintained. We consider the project a successful
demonstration of an abstract, JIT- and alternative-Python-friendly C API for
Python extensions, and of a possible path for evolving CPython without
exposing implementation details.

Several of the ideas explored by HPy have since become part of CPython's C
API evolution, whether directly inspired by HPy or developed in parallel:
opaque interpreter and type structures, getter/setter APIs instead of direct
field access, converting macros to functions, and avoiding borrowed references
in new APIs. See [PEP 620](https://peps.python.org/pep-0620/), [PEP
670](https://peps.python.org/pep-0670/), and [PEP
674](https://peps.python.org/pep-0674/) for some of this work. HPy's
perspective was also presented at the [CPython core developer sprint and C API
Summit in Brno in 2023](https://hpyproject.org/blog/posts/2023/10/hpy-meetup-2023/),
along with the [presentation slides](https://hpyproject.org/talks/2023/10/things_to_learn_from_hpy.pdf).

Ideas that require a substantially new API are being explored by the
[PyNI PEP work](https://github.com/py-ni/peps/blob/main/peps/pep-08xx.rst).
That work is progressing gradually as community time and support become
Contributions are welcome from people seriously interested in
moving this work forward; experience with CPython development and C extensions
is helpful,
and we are happy to help onboard new contributors alongside the existing
maintainers and authors.

## Summary

HPy is a better API for extending Python
in C. The old C API is specific to the current implementation of CPython.
It exposes a lot of internal details which makes it hard to:

  - implement it for other Python implementations (e.g. PyPy, GraalPy,
    Jython, IronPython, etc.).
  - experiment with new things inside CPython itself: e.g. using a GC
    instead of refcounting, or to remove the GIL
  - guarantee binary stability

HPy is a specification of a new API and ABI for extending Python that is
Python implementation agnostic and designed to hide and abstract internal
details such that it:

  - can stay binary compatible even if the underlying Python internals change significantly
  - does not hinder internal progress of CPython and other Pythons

Please read the [documentation](https://docs.hpyproject.org/en/latest/overview.html)
for more details on HPy motivation, goals, and features, for example:

  - debug mode for better developer experience
  - support for incremental porting from CPython API to HPy
  - CPython ABI for raw performance on CPython
  - and others

Do you want to see how HPy API looks in code? Check out
our [quickstart example](https://docs.hpyproject.org/en/latest/quickstart.html).

You may also be interested in HPy's
[API reference](https://docs.hpyproject.org/en/latest/api-reference/index.html).

This repository contains the API and ABI specification and implementation
for the CPython interpreter. Other interpreters that support HPy natively: GraalPy
and PyPy, provide their own builtin HPy implementations.


## Why should I care about this stuff?

  - the existing C API is becoming a problem for CPython and for the
    evolution of the language itself: this project makes it possible to make
    experiments which might be "officially" adopted in the future

  - for PyPy, it will give obvious speed benefits: for example, data
    scientists will be able to get the benefit of fast C libraries *and* fast
    Python code at the same time, something which is hard to achieve now

  - the current implementation is too tied to CPython and proved to be a
    problem for almost all the other alternative implementations. Having an
    API which is designed to work well on two different implementations will
    make the job much easier for future ones: going from 2 to N is much easier
    than going from 1 to 2

  - arguably, it will be easier to learn and understand than the massive
    CPython C API

See also [Python Performance: Past, Present,
Future](https://github.com/vstinner/talks/raw/main/2019-EuroPython/python_performance.pdf)
by Victor Stinner.


## What does `HPy` mean?

The "H" in `HPy` stands for "handle": one of the key idea of the new API is to
use fully opaque handles to represent and pass around Python objects.
