HPy API
=======

.. warning::
   HPy is still in the early stages of development and as such the API is
   subsequent to changes

Handles
-------

The "H" in HPy stands for **handle**, which is a central concept: handles are
used to hold a C reference to Python objects, and they are represented by the
C ``HPy`` type.  They play the same role as ``PyObject *`` in the Python/C
API, albeit with some important differences which are detailed below.

When they are no longer needed, handles must be closed by calling
``HPy_Close``, which plays more or less the same role as ``Py_DECREF``.
Similarly, if you need a new handle for an existing object, you can duplicate
it by calling ``HPy_Dup``, which plays more or less the same role as
``Py_INCREF``.

The concept of handles is certainly not unique to HPy. Other examples include
Unix file descriptors, where you have ``dup()`` and ``close()``, and Windows'
``HANDLE``, where you have ``DuplicateHandle()`` and ``CloseHandle()``.


Handles vs ``PyObject *``
~~~~~~~~~~~~~~~~~~~~~~~~~

.. XXX I don't like this sentence, but I can't come up with anything better
   right now. Please rephrase/rewrite :)

The biggest difference is that in the old Python/C API, multiple ``PyObject
*`` references to the same objects are completely equivalent to each other. In
particular, it does not matter on which particular reference you call
``Py_INCREF`` and ``Py_DECREF``, as long as the total number of increfs and
decrefs is the same at the end of the object lifetime.

For example, the following is a perfectly valid piece of Python/C code::

  void foo(void)
  {
      PyObject *x = PyLong_FromLong(42);  // implicit INCREF on x
      PyObject *y = x;
      Py_INCREF(y);                       // INCREF on y
      /* ... */
      Py_DECREF(x);
      Py_DECREF(x);                       // two DECREF on x
  }

In HPy, each handle must be closed independently. The example above becomes::

  void foo(HPyContext ctx)
  {
      HPy x = HPyLong_FromLong(ctx, 42);
      HPy y = HPy_Dup(ctx, x);
      /* ... */
      // we need to close x and y independently
      HPy_Close(ctx, x);
      HPy_Close(ctx, y);
  }

Calling ``HPy_Close()`` on the same handle twice is an error.  Forgetting to
call ``HPy_Close()`` on a handle results in a memory leak.  When running in
:ref:`debug mode`, HPy actively checks that you that you don't close a handle
twice and that you don't forget to close any.


.. note::
  The debug mode is a good example of how powerful it is to decouple the
  lifetime of handles and the lifetime of an objects.  If you find a memory
  leak on CPython, you know that you are missing a ``Py_DECREF`` somewhere but
  the only way to find the corresponding ``Py_INCREF`` is to manually and
  carefully study the source code.  On the other hand, if you forget to call
  ``HPy_Close()``, the HPy debug mode is able to tell the precise code
  location which created the unclosed handle.


The other important difference is that Python/C guarantees that multiple
references to the same object results in the very same ``PyObject *`` pointer.
Thus, it is possible to compare C pointers by equality to check whether they
point to the same object::

    void is_same_object(PyObject *x, PyObject *y)
    {
        return x == y;
    }

On the other hand, in HPy, each handle is independent and it is common to have
two different handles which point to the same underlying object, so comparing
two handles directly is ill-defined.  To prevent this kind of common error
(especially when porting existing code to HPy), the ``HPy`` C type is opaque
and the C compiler actively forbids comparisons between them.  To check for
identity, you can use ``HPy_Is()``::

    void is_same_object(HPyContext ctx, HPy x, HPy y)
    {
        // return x == y; // compilation error!
        return HPy_Is(ctx, x, y);
    }

.. note::
   The main benefit of the semantics of handles is that it allows
   implementations to use very different models of memory management.  On
   CPython, implementing handles is trivial because ``HPy`` is basically
   ``PyObject *`` in disguise, and ``HPy_Dup()`` and ``HPy_Close()`` are just
   aliases for ``Py_INCREF`` and ``Py_DECREF``.

   Contrarily to CPython, PyPy does not use reference counting for memory
   management: instead, it uses a *moving GC*, which means that the address of
   an object might change during its lifetime, and makes it hard to implement
   a semantics like ``PyObject *`` where the address is directly exposed to
   the user.  HPy solves this problem: handles are integers which represent
   indices into a list, which is itself managed by the GC. When an object
   moves, the GC fixes the address into the list, without having to touch all
   the handles which have been passed to C.
