Porting guide
=============

Porting ``PyObject *`` to HPy API constructs
--------------------------------------------

While in CPython one always uses ``PyObject *`` to reference to Python objects,
in HPy there are several types of handles that should be used depending on the
life-time of the handle: ``HPy``, ``HPyField``, and ``HPyGlobal``.

- `HPy` represents short lived handles that live no longer than the duration of
  one call from Python to HPy extension function. Rule of thumb: use for local
  variables, arguments, and return values.

- `HPyField` represents handles that are Python object struct fields, i.e.,
  live in native memory attached to some Python object.

- `HPyGlobal` represents handles stored in C global variables. `HPyGlobal`
  can provide isolation between subinterpreters.

**WARNING**: never use a local variable of type ``HPyField``, for any reason!
If the GC kicks in, it might become invalid and become a dangling pointer.

**WARNING**: never store `HPy` handles to a long-lived memory, for example: C global
variables or Python object structs.

The ``HPy``/``HPyField`` dichotomy might seem arbitrary at first, but it is
needed to allow Python implementations to use a moving GC, such as PyPy. It is
easier to explain and understand the rules by thinking about how a moving GC
interacts with the C code inside an HPy extension.

It is worth remembering that during the collection phase, a moving GC might
move an existing object to another memory location, and in that case it needs
to update all the places which store a pointer to it.  In
order to do so, it needs to *know* where the pointers are. If there is a local C
variable which is unknown to the GC but contains a pointer to a GC-managed
object, the variable will point to invalid memory as soon as the object is
moved.

Back to ``HPy`` vs ``HPyField`` vs ``HPyGlobal``:

  * ``HPy`` handles must be used for all C local variables, function arguments
    and function return values. They are supposed to be short-lived and closed
    as soon as they are no longer needed. The debug mode will report a
    long-lived ``HPy`` as a potential memory leak.

  * In PyPy and GraalPython, `HPy` handles are implemented using an indirection:
    they are indexes inside a big list of GC-managed objects: this big list is
    tracked by the GC, so when an object move its pointer is correctly updated.

  * ``HPyField`` is for long-lived references, and the GC must be aware of
    their location in memory. In PyPy, an ``HPyField`` is implemented as a
    direct pointer to the object, and thus we need a way to inform the GC
    where it is in memory, so that it can update its value upon moving: this
    job is done by ``tp_traverse``, as explained in the next section.

  * ``HPyGlobal`` is for long-lived references that are supposed to be closed
    implicitly when the module is unloaded (once module unloading is actually
    implemented). ``HPyGlobal`` provides indirection to isolate subinterpreters.
    Implementation wise, ``HPyGlobal`` will usually contain index to a table
    with Python objects stored in the interpreter state.

  * On CPython without subinterpreters support, ``HPy``, ``HPyGlobal``,
    and ``HPyField`` are implemented as ``PyObject *``.

  * On CPython with subinterpreters support, ``HPyGlobal`` will be implemented
    by an indirection through the interpreter state. Note that thanks to the HPy
    design, switching between this and the more efficient implementation without
    subinterpreter support will not require rebuilding of the extension (in HPy
    universal mode), nor rebuilding of CPython.

**IMPORTANT**: if you write a custom type having ``HPyField`` s, you **MUST**
also write a ``tp_traverse`` slot. Note that this is different than the old
Python/C API, where you need ``tp_traverse`` only under certain
conditions. See the next section for more details.

**IMPORTANT**: the contract of ``tp_traverse`` is that it must visit all the
``HPyFields`` contained within given struct, or more precisely "owned" by given
Python object (in the sense of the "owner" argument to ``HPyField_Store``), and
nothing more or less. Some Python implementations may choose to not call the
provided ``tp_traverse`` if they know how to visit all the ``HPyFields`` by other
means (for example, when they track them internally already). The debug mode will
check this contract.

``tp_traverse``, ``tp_clear``, ``Py_TPFLAGS_HAVE_GC``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let's quote the Python/C documentation about `GC support
<https://docs.python.org/3/c-api/gcsupport.html>`_

  Python’s support for detecting and collecting garbage which involves
  circular references requires support from object types which are
  “containers” for other objects which may also be containers. Types which do
  not store references to other objects, or which only store references to
  atomic types (such as numbers or strings), do not need to provide any
  explicit support for garbage collection.

A good rule of thumb is that if your type contains ``PyObject *`` fields, you
need to:

  1. provide a ``tp_traverse`` slot;

  2. provide a ``tp_clear`` slot;

  3. add the ``Py_TPFLAGS_GC`` to the ``tp_flags``.


However, if you know that your ``PyObject *`` fields will contain only
"atomic" types, you can avoid these steps.

In HPy the rules are slightly different:

  1. if you have a field of type ``HPyField``, you always **MUST** provide a
     ``tp_traverse``. This is needed so that a moving GC can track the
     relevant areas of memory. However, you **MUST NOT** rely on
     ``tp_traverse`` to be called;

  2. ``tp_clear`` does not exist. On CPython, ``HPy`` automatically generates
     one for you, by using ``tp_traverse`` to know which are the fields to
     clear. Other implementations are free to ignore it, if it's not needed;

  3. ``HPy_TPFLAGS_GC`` is still needed, especially on CPython. If you don't
     specify it, your type will not be tracked by CPython's GC and thus it
     might cause memory leaks if it's part of a reference cycle.  However,
     other implementations are free to ignore the flag and track the objects
     anyway, if their GC implementation allows it.


``tp_dealloc`` and ``Py_DECREF``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Generally speaking, if you have one or more ``PyObject *`` fields in the old
Python/C, you must provide a ``tp_dealloc`` slot where you ``Py_DECREF`` all
of them. In HPy this is not needed and will be handled automatically by the
system.

In particular, when running on top of CPython, HPy will automatically provide
a ``tp_dealloc`` which decrefs all the fields listed by ``tp_traverse``.



PyModule_AddObject
------------------

``PyModule_AddObject()`` is replaced with a regular ``HPy_SetAttr_s()``. There
is no ``HPyModule_AddObject()`` because it has an unusual refcount behaviour
(stealing a reference but only when it returns 0).

Py_tp_dealloc
-------------

``Py_tp_dealloc`` becomes ``HPy_tp_destroy``. We changed the name a little bit
because only "lightweight" destructors are supported. Use ``tp_finalize`` if
you really need to do things with the context or with the handle of the
object.


Py_tp_methods, Py_tp_members and Py_tp_getset
---------------------------------------------

``Py_tp_methods``, ``Py_tp_members`` and ``Py_tp_getset`` are no longer needed.
Methods, members and getsets are specified "flatly" together with the other
slots, using the standard mechanism of ``HPyDef_{METH,MEMBER,GETSET}`` and
``HPyType_Spec.defines``.


PyList_New/PyList_SET_ITEM
---------------------------

``PyList_New(5)``/``PyList_SET_ITEM()`` becomes::

    HPyListBuilder builder = HPyListBuilder_New(ctx, 5);
    HPyListBuilder_Set(ctx, builder, 0, h_item0);
    ...
    HPyListBuilder_Append(ctx, builder, h_item5);
    ...
    HPy h_list = HPyListBuilder_Build(ctx, builder);

For lists of (say) integers::

    HPyListBuilder_i builder = HPyListBuilder_i_New(ctx, 5);
    HPyListBuilder_i_Set(ctx, builder, 0, 42);
    ...
    HPy h_list = HPyListBuilder_i_Build(ctx, builder);

And similar for building tuples or bytes


PyObject_Call and PyObject_CallObject
-------------------------------------

Both ``PyObject_Call`` and ``PyObject_CallObject`` are replaced by
``HPy_CallTupleDict(callable, args, kwargs)`` in which either or both of
``args`` and ``kwargs`` may be null handles.

``PyObject_Call(callable, args, kwargs)`` becomes::

    HPy result = HPy_CallTupleDict(ctx, callable, args, kwargs);

``PyObject_CallObject(callable, args)`` becomes::

    HPy result = HPy_CallTupleDict(ctx, callable, args, HPy_NULL);

If ``args`` is not a handle to a tuple or ``kwargs`` is not a handle to a
dictionary, ``HPy_CallTupleDict`` will return ``HPy_NULL`` and raise a
``TypeError``. This is different to ``PyObject_Call`` and
``PyObject_CallObject`` which may segfault instead.

Buffers
-------

The buffer API in HPy is implemented using the ``HPy_buffer`` struct, which looks
very similar to ``Py_buffer`` (refer to the `CPython documentation
<https://docs.python.org/3.6/c-api/buffer.html#buffer-structure>`_ for the
meaning of the fields)::

    typedef struct {
        void *buf;
        HPy obj;
        HPy_ssize_t len;
        HPy_ssize_t itemsize;
        int readonly;
        int ndim;
        char *format;
        HPy_ssize_t *shape;
        HPy_ssize_t *strides;
        HPy_ssize_t *suboffsets;
        void *internal;
    } HPy_buffer;

Buffer slots for HPy types are specified using slots ``HPy_bf_getbuffer`` and
``HPy_bf_releasebuffer`` on all supported Python versions, even though the
matching PyType_Spec slots, ``Py_bf_getbuffer`` and ``Py_bf_releasebuffer``, are
only available starting from CPython 3.9.
