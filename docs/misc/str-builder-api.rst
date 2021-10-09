bytes/str building API
=======================

We need to design an HPy API to build ``bytes`` and ``str`` objects. Before making
any proposal, it is useful to understand:

1. What is the current API to build strings.

2. What are the constraints for alternative implementations and the problems
   of the current C API.

3. What are the patterns used to build string objects in the existing
   extensions.

Some terminology:

    * "string" means both ``bytes`` and ``str`` objects
    * "unicode" or "unicode string" indicates ``str``

.. note::
   In this document we are translating ``PyUnicode_*`` functions into
   ``HPyStr_*``. See `issue #213 <https://github.com/hpyproject/hpy/issues/213>`_
   for more discussion about the naming convention.

.. note::
   The goal of the document is only to describe the current CPython API and
   its real-world usage. For a discussion about how to design the equivalent
   HPy API, see `issue #214 <https://github.com/hpyproject/hpy/issues/214>`_


Current CPython API
--------------------

Bytes
~~~~~

There are essentially two ways to build ``bytes``:

1. Copy the content from an existing C buffer:

.. code-block:: c

    PyObject* PyBytes_FromString(const char *v);
    PyObject* PyBytes_FromStringAndSize(const char *v, Py_ssize_t len);
    PyObject* PyBytes_FromFormat(const char *format, ...);


2. Create an uninitialized buffer and fill it manually:

.. code-block:: c

    PyObject s = PyBytes_FromStringAndSize(NULL, size);
    char *buf = PyBytes_AS_STRING(s);
    strcpy(buf, "hello");

(1) is easy for alternative implementations and we can probably provide an HPy
equivalent without changing much, so we will concentrate on (2): let's call it
"raw-buffer API".

Unicode
~~~~~~~

Similarly to ``bytes``, there are several ways to build a ``str``:

.. code-block:: c

    PyObject* PyUnicode_FromString(const char *u);
    PyObject* PyUnicode_FromStringAndSize(const char *u, Py_ssize_t size);
    PyObject* PyUnicode_FromKindAndData(int kind, const void *buffer, Py_ssize_t size);
    PyObject* PyUnicode_FromFormat(const char *format, ...);
    PyObject* PyUnicode_New(Py_ssize_t size, Py_UCS4 maxchar);


.. note::
   ``PyUnicode_FromString{,AndSize}`` take an UTF-8 string in input

The following functions are used to initialize an uninitialized object, but I
could not find any usage of them outside CPython itself, so I think they can
be safely ignored for now:

.. code-block:: c

    Py_ssize_t PyUnicode_Fill(PyObject *unicode, Py_ssize_t start, Py_ssize_t length, Py_UCS4 fill_char);
    Py_ssize_t PyUnicode_CopyCharacters(PyObject *to, Py_ssize_t to_start, PyObject *from, Py_ssize_t from_start, Py_ssize_t how_many);


There are also a bunch of API functions which have been deprecated (see `PEP
623 <https://www.python.org/dev/peps/pep-0623/>`_ and `PEP 624
<https://www.python.org/dev/peps/pep-0624/>`_) so we will not take them into
account. The deprecated functions include but are not limited to:

.. code-block:: c

    PyUnicode_FromUnicode
    PyUnicode_FromStringAndSize(NULL,...) // use PyUnicode_New instead
    PyUnicode_AS_UNICODE
    PyUnicode_AS_DATA
    PyUnicode_READY


Moreover, CPython 3.3+ adopted a flexible string represenation (`PEP 393
<https://www.python.org/dev/peps/pep-0393/>`_) which means that the underlying
buffer of ``str`` objects can be an array of 1-byte, 2-bytes or 4-bytes
characters (the so called "kind").

``str`` objects offer a raw-buffer API, but you need to call the appropriate
function depending on the kind, returning buffers of different types:

.. code-block:: c

    typedef uint32_t Py_UCS4;
    typedef uint16_t Py_UCS2;
    typedef uint8_t Py_UCS1;
    Py_UCS1* PyUnicode_1BYTE_DATA(PyObject *o);
    Py_UCS2* PyUnicode_2BYTE_DATA(PyObject *o);
    Py_UCS4* PyUnicode_4BYTE_DATA(PyObject *o);


Uninitialized unicode objects are created by calling ``PyUnicode_New(size,
maxchar)``, where ``maxchar`` is the maximum allowed value of a character
inside the string, and determines the kind. So, in cases in which ``maxchar``
is known in advance, we can predict at compile time what will be the kind of
the string and write code accordingly. E.g.:

.. code-block:: c

    // ASCII only --> kind == PyUnicode_1BYTE_KIND
    PyObject *s = PyUnicode_New(size, 127);
    Py_UCS1 *buf = PyUnicode_1BYTE_DATA(s);
    strcpy(buf, "hello");


.. note::
   CPython distinguishes between ``PyUnicode_New(size, 127)`` and
   ``PyUnicode_New(size, 255)``: in both cases the kind is
   ``PyUnicode_1BYTE_KIND``, but the former also sets a flag to indicate that
   the string is ASCII-only.

There are cases in which you don't know the kind in advance because you are
working on generic data. To solve the problem in addition to the raw-buffer
API, CPython also offers an "Opaque API" to write a char inside an unicode:

.. code-block:: c

    int PyUnicode_WriteChar(PyObject *unicode, Py_ssize_t index, Py_UCS4 character)
    void PyUnicode_WRITE(int kind, void *data, Py_ssize_t index, Py_UCS4 value)

Note that the character to write is always ``Py_UCS4``, so
``_WriteChar``/``_WRITE`` have logic to do something different depending on
the kind.

.. note::
    ``_WRITE`` is a macro, and its implementation contains a ``switch(kind)``:
    I think it is designed with the explicit goal of allowing the compiler to
    hoist the ``switch`` outside a loop in which we repeatedly call
    ``_WRITE``. However, it is worth noting that I could not find any code
    using it outside CPython itself, so it's probably something which we don't
    need to care of for HPy.


Raw-buffer vs Opaque API
---------------------------

There are two ways to initialize a non-initialized string object:

- **Raw-buffer API**: get a C pointer to the memory and fill it directly:
  ``PyBytes_AsString``, ``PyUnicode_1BYTE_DATA``, etc.

- **Opaque API**: call special functions API to fill the content, without
  accessing the buffer directly: e.g., ``PyUnicode_WriteChar``.

From the point of view of the implementation, a completely opaque API gives
the most flexibility in terms of how to implement a builder and/or a string.
A good example is PyPy's ``str`` type, which uses UTF-8 as the internal
representation. A completely opaque ``HPyStrBuilder`` could allow PyPy to fill
directly its internal UTF-8 buffer (at least in simple cases). On the other
hand, a raw-buffer API would force PyPy to store the UCS{1,2,4} bytes in a
temporary buffer and convert them to UTF-8 during the ``build()`` phase.

On the other hand, from the point of view of the C programmer it is easier to
have direct access the memory. This allows to:

- use ``memcpy()`` to copy data into the buffer

- pass the buffer directly to other C functions which write into it (e.g.,
  ``read()``)

- use standard C patterns such as ``*p++ = ...`` or similar.


Problems and constraints
------------------------

``bytes`` and ``str`` are objects are immutable: the biggest problem of the
current API boils down to the fact that the API allows to construct objects
which are not fully initialized and to mutate them during a
not-well-specificed "initialization phase".

Problems for alternative implementations:

1. it assumes that the underlying buffer **can** be mutated. This might not be
   always the case, e.g. if you want to use a Java string or an RPython string
   as the data buffer. This might also lead to unnecessary copies.

2. It makes harder to optimize the code: e.g. a JIT cannot safely assume that
   a string is actually immutable.

3. It interacts badly with a moving GC, because we need to ensure that ``buf``
   doesn't move.

Introducing a builder solves most of the problems, because it introduces a
clear separation between the mutable and immutable phases.


Real world usage
-----------------

In this section we analyze the usage of some string building API in
real world code, as found in the `Top 4000 PyPI packages
<https://github.com/hpyproject/top4000-pypi-packages>`_.

PyUnicode_New
~~~~~~~~~~~~~

This is the recommended "modern" way to create ``str`` objects but it's not
widely used outside CPython. A simple ``grep`` found only 17 matches in the
4000 packages, although some are in very important packages such as
`cffi <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0021-cffi-1.14.5/c/wchar_helper_3.h#L36>`_,
``markupsafe``
(`1 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L106>`__,
`2 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L132>`__,
`3 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L158>`__)
and ``simplejson``
(`1 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0096-simplejson-3.17.2/simplejson/_speedups.c#L517>`__,
`2 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0096-simplejson-3.17.2/simplejson/_speedups.c#L3330>`__).

In all the examples linked above, ``maxchar`` is hard-coded and known at
compile time.

There are only four usages of ``PyUnicode_New`` in which ``maxchar`` is
actually unknown until runtime, and it is curious to note that the first three
are in runtime libraries used by code generators:

  1. `mypyc <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0277-mypy-0.812/mypyc/lib-rt/str_ops.c#L22>`__

  2. `Cython <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0158-Cython-0.29.23/Cython/Utility/StringTools.c#L829>`__

  3. `siplib <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top4000/1236-PyQt5_sip-12.9.0/siplib.c#L12808>`__

  4. `PyICU <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top4000/2601-PyICU-2.7.3/common.cpp#L213>`__:
     this is the only non-runtime library usage of it, and it's used to
     implement a routine to create a ``str`` object from an UTF-16 buffer.

For HPy, we should at lest consider the opportunity to design special APIs for
the cases in which ``maxchar`` is known in advance,
e.g. ``HPyStrBuilder_ASCII``, ``HPyStrBuilder_UCS1``, etc., and evaluate
whether this would be beneficial for alternative implementations.

Create empty strings
~~~~~~~~~~~~~~~~~~~~~

A special case is ``PyUnicode_New(0, 0)``, which contructs an empty ``str``
object.  CPython special-cases it to always return a prebuilt object.

This pattern is used a lot inside CPython but only once in 3rd-party extensions, in the ``regex`` library (
`1 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L19486>`__,
`2 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L19516>`__).

Other ways to build empty strings are ``PyUnicode_FromString("")`` which is used 27 times and ``PyUnicode_FromStringAndSize("", 0)`` which is used only `once
<https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0268-pyodbc-4.0.30/src/textenc.cpp#L144>`_.

For HPy, maybe we should just have a ``ctx->h_EmptyStr`` and
``ctx->h_EmptyBytes``?

PyUnicode_From*, PyUnicode_Decode*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions of the ``PyUnicode_From*`` and ``PyUnicode_Decode*`` families should
be easy to adapt to HPy, so we won't discuss them in detail. However, here is
the of matches found by grep for each function, to get an idea of how much
each is used:

``PyUnicode_From*`` family::

  Documented:
    964 PyUnicode_FromString
    259 PyUnicode_FromFormat
    125 PyUnicode_FromStringAndSize
     58 PyUnicode_FromWideChar
     48 PyUnicode_FromEncodedObject
     17 PyUnicode_FromKindAndData
      9 PyUnicode_FromFormatV

  Undocumented:
      7 PyUnicode_FromOrdinal

  Deprecated:
     66 PyUnicode_FromObject
     45 PyUnicode_FromUnicode

``PyUnicode_Decode*`` family::

    143 PyUnicode_DecodeFSDefault
    114 PyUnicode_DecodeUTF8
     99 PyUnicode_Decode
     64 PyUnicode_DecodeLatin1
     51 PyUnicode_DecodeASCII
     12 PyUnicode_DecodeFSDefaultAndSize
     10 PyUnicode_DecodeUTF16
      8 PyUnicode_DecodeLocale
      6 PyUnicode_DecodeRawUnicodeEscape
      3 PyUnicode_DecodeUTF8Stateful
      2 PyUnicode_DecodeUTF32
      2 PyUnicode_DecodeUnicodeEscape


Raw-buffer access
~~~~~~~~~~~~~~~~~

Most of the real world packages use the raw-buffer API to initialize ``str``
objects, and very often in a way which can't be easily replaced by a fully
opaque API.

Example 1, ``markupsafe``: the
`DO_ESCAPE <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L35>`_
macro takes a parameter called ``outp`` which is obtained by calling
``PyUnicode*BYTE_DATA``
(`1BYTE <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L112>`_,
(`2BYTE <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L137>`_,
(`4BYTE <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0024-MarkupSafe-2.0.1/src/markupsafe/_speedups.c#L163>`_).
``DO_ESCAPE`` contains code like this, which would be hard to port to a fully-opaque API:

.. code-block:: c

    memcpy(outp, inp-ncopy, sizeof(*outp)*ncopy); \
    outp += ncopy; ncopy = 0; \
    *outp++ = '&'; \
    *outp++ = '#'; \
    *outp++ = '3'; \
    *outp++ = '4'; \
    *outp++ = ';'; \
    break; \

Another interesting example is
`pybase64 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top4000/1925-pybase64-1.1.4/pybase64/_pybase64.c#L320-349>`_.
After removing the unnecessary stuff, the logic boils down to this:

.. code-block:: c

    out_len = (size_t)(((buffer.len + 2) / 3) * 4);
    out_object = PyUnicode_New((Py_ssize_t)out_len, 127);
    dst = (char*)PyUnicode_1BYTE_DATA(out_object);
    ...
    base64_encode(buffer.buf, buffer.len, dst, &out_len, libbase64_simd_flag);

Note that ``base64_encode`` is an external C function which writes stuff into
a ``char *`` buffer, so in this case it is **required** to use the raw-buffer
API, unless you want to allocate a temporary buffer and copy chars one-by-one
later.

There are other examples similar to these, but I think there is already enough
evidence that HPy **must** offer a raw-buffer API in addition to a
fully-opaque one.


Typed vs untyped raw-buffer writing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To initialize a ``str`` object using the raw-buffer interface, you need to get
a pointer to the buffer.  The vast majority of code uses
``PyUnicode_{1,2,4}BYTE_DATA`` to get a buffer of type ``Py_UCS{1,2,4}*`` and
write directly to it:

.. code-block:: c

    PyObject *s = PyUnicode_New(size, 127);
    Py_UCS1 *buf = PyUnicode_1BYTE_DATA(s);
    buf[0] = 'H';
    buf[1] = 'e';
    buf[2] = 'l';
    ...

The other way to get a pointer to the raw-buffer is to call
``PyUnicode_DATA()``, which returns a ``void *``: the only reasonable way to
write something in this buffer is to ``memcpy()`` the data from another
``str`` buffer of the same kind. This technique is used for example by
`CPython's textio.c <https://github.com/antocuni/cpython/blob/7b3ab5921fa25ed8b97b6296f97c5c78aacf5447/Modules/_io/textio.c#L344>`_.

Outside CPython, the only usage of this technique is inside cython's helper
function `__Pyx_PyUnicode_Join <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0158-Cython-0.29.23/Cython/Utility/StringTools.c#L857>`_.

This probably means that we don't need to offer untyped raw-buffer writing for
HPy. If we really need to support the ``memcpy`` use case, we can probably
just offer a special function in the builder API.

PyUnicode_WRITE, PyUnicode_WriteChar
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Outside CPython, ``PyUnicode_WRITE()`` is used only inside Cython's helper
functions
(`1 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0158-Cython-0.29.23/Cython/Utility/StringTools.c#L865>`__,
`2 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0158-Cython-0.29.23/Cython/Utility/StringTools.c#L914-L926>`__).
Considering that Cython will need special support for HPy anyway, this means
that we don't need an equivalent of ``PyUnicode_WRITE`` for HPy.

Similarly, ``PyUnicode_WriteChar()`` is used only once, inside
`JPype <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0546-JPype1-1.2.1/native/python/jp_pythontypes.cpp#L196>`_.


PyUnicode_Join
~~~~~~~~~~~~~~

All the API functions listed above require the user to know in advance the
size of the string: ``PyUnicode_Join()`` is the only native API call which
allows to build a string whose size is not known in advance.

Examples of usage are found in ``simplejson``
(`1 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0096-simplejson-3.17.2/simplejson/_speedups.c#L779>`__,
`2 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top100/0096-simplejson-3.17.2/simplejson/_speedups.c#L1033>`__),
`pycairo <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0759-pycairo-1.20.0/cairo/path.c#L156>`__,
``regex``
(`1 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L19492>`__,
`2 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L22674>`__,
`3 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L22768>`__,
`4 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L19440>`__,
`5 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L22495>`__,
`6 <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L22589>`__)
and others, for a total of 25 grep matches.


.. note::

   Contrarily to its unicode equivalent, ``PyBytes_Join()`` does not
   exist. There is ``_PyBytes_Join()`` which is private and undocumented, but
   some extensions rely on it anyway:
   `Cython <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0158-Cython-0.29.23/Cython/Utility/StringTools.c#L795>`__,
   `regex <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top1000/0119-regex-2021.4.4/regex_3/_regex.c#L19501>`__,
   `dulwich <https://github.com/hpyproject/top4000-pypi-packages/blob/0cd919943a007f95f4bf8510e667cfff5bd059fc/top4000/1424-dulwich-0.20.23/dulwich/_pack.c#L62>`__.

In theory, alternative implementaions should be able to provide a more
efficient way to achieve the goal. E.g. for pure Python code PyPy offers
``__pypy__.builders.StringBuilder`` which is faster than both ``StringIO`` and
``''.join``, so maybe it might make sense to offer a way to use it from C.
