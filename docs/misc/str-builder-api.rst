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

    * "string" indicates either ``bytes`` or ``str`` objects aaa
    * "unicode" or "unicode string" indicates ``str``

.. note::
   In this issue we are translating ``PyUnicode_*`` functions into
   ``HPyStr_*``. See issue #xxx for more discussion about the naming convention.

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

    PyObject *PyUnicode_FromString(const char *u);
    PyObject* PyUnicode_FromStringAndSize(const char *u, Py_ssize_t size);
    PyObject* PyUnicode_FromKindAndData(int kind, const void *buffer, Py_ssize_t size);
    PyObject* PyUnicode_FromFormat(const char *format, ...);
    PyObject* PyUnicode_New(Py_ssize_t size, Py_UCS4 maxchar);


.. note::
   ``PyUnicode_FromString{,AndSize}`` take an UTF-8 string in input

The following functions are used to initialize an uninitialed object, but I
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

    PyUnicode_FromStringAndSize(NULL,...) // use PyUnicode_New instead
    PyUnicode_AS_UNICODE
    PyUnicode_AS_DATA
    PyUnicode_FromUnicode
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
maxchar)``, where ``maxchar`` is the maximum allowed value of each
character. So, in cases in which ``maxchar`` is known in advance, we can
predict at compile time what will be the kind of the string and write code
accordingly. E.g.:

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
