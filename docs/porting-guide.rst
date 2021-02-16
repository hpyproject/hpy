Porting guide
=============

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
