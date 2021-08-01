Porting guide
=============

Porting ``PyObject *``: ``HPy`` vs ``HPyField``
-----------------------------------------------

The rule of thumb is:

  * for local variables, function arguments and return types: use ``HPy``;

  * for struct fields, use ``HPyField``.


The ``HPy``/``HPyField`` dichotomy might seem arbirary at first, but it is
needed to allow Python implementations to use a moving GC, such as PyPy. It is
easier to explain and understand the rules by thinking at how a moving GC
interacts with the C code inside an HPy extension.

It is worth remembering that during the collection phase, a moving GC might
move an existing object to another memory location, and in that case it needs
to update all the places which store a pointer to it.  It is obvious that in
order to do so, it needs to *know* where they are. If there is a local C
variable which is unknown to the GC but contains a pointer to a GC-managed
object, the variable will point to invalid memory as soon as the object is
moved.

Back to ``HPy`` vs ``HPyField``:

  * ``HPy`` handles must be used for all C local variables, function arguments
    and function return values. They are supposed to be short-lived and closed
    as soon as they are no longer needed. The debug mode will report a
    long-lived ``HPy`` as a potential memory leak.

  * In PyPy, they are implemented using an indirection: they are indexes
    inside a big list of GC-managed objects: this big list is tracked by the
    GC, so when an object move its pointer is correctly updated.

  * ``HPyField`` is for long-lived references, and the GC must be aware of
    their location in memory. In PyPy, an ``HPyField`` is implemented as a
    direct pointer to the object, and since the GC is aware it automatically
    updates its value upon moving.

  * On CPython, both ``HPy`` and ``HPyField`` are implemented as ``PyObject *``.

**IMPORTANT**: if you write a custom type having ``HPyField``s, you **MUST**
 also write a ``tp_traverse`` slot. Note that this is different than CPython,
 where you need ``tp_traverse`` only under certain conditions. See the next
 section for more details.

``tp_traverse`` and ``Py_TPFLAGS_HAVE_GC``
------------------------------------------

XXX


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
