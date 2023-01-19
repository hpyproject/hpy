Porting Guide
=============

Porting ``PyObject *`` to HPy API constructs
--------------------------------------------

While in CPython one always uses ``PyObject *`` to reference to Python objects,
in HPy there are several types of handles that should be used depending on the
life-time of the handle: ``HPy``, ``HPyField``, and ``HPyGlobal``.

- ``HPy`` represents short lived handles that live no longer than the duration of
  one call from Python to HPy extension function. Rule of thumb: use for local
  variables, arguments, and return values.

- ``HPyField`` represents handles that are Python object struct fields, i.e.,
  live in native memory attached to some Python object.

- ``HPyGlobal`` represents handles stored in C global variables. ``HPyGlobal``
  can provide isolation between subinterpreters.

.. warning:: Never use a local variable of type ``HPyField``, for any reason! If
    the GC kicks in, it might become invalid and become a dangling pointer.

.. warning:: Never store `HPy` handles to a long-lived memory, for example: C
    global variables or Python object structs.

The ``HPy``/``HPyField`` dichotomy might seem arbitrary at first, but it is
needed to allow Python implementations to use a moving GC, such as PyPy. It is
easier to explain and understand the rules by thinking about how a moving GC
interacts with the C code inside an HPy extension.

It is worth remembering that during the collection phase, a moving GC might
move an existing object to another memory location, and in that case it needs
to update all the places which store a pointer to it.  In order to do so, it
needs to *know* where the pointers are. If there is a local C variable which is
unknown to the GC but contains a pointer to a GC-managed object, the variable
will point to invalid memory as soon as the object is moved.

Back to ``HPy`` vs ``HPyField`` vs ``HPyGlobal``:

  * ``HPy`` handles must be used for all C local variables, function arguments
    and function return values. They are supposed to be short-lived and closed
    as soon as they are no longer needed. The debug mode will report a
    long-lived ``HPy`` as a potential memory leak.

  * In PyPy and GraalPython, ``HPy`` handles are implemented using an
    indirection: they are indexes inside a big list of GC-managed objects: this
    big list is tracked by the GC, so when an object moves its pointer is
    correctly updated.

  * ``HPyField`` is for long-lived references, and the GC must be aware of
    their location in memory. In PyPy, an ``HPyField`` is implemented as a
    direct pointer to the object, and thus we need a way to inform the GC
    where it is in memory, so that it can update its value upon moving: this
    job is done by ``tp_traverse``, as explained in the next section.

  * ``HPyGlobal`` is for long-lived references that are supposed to be closed
    implicitly when the module is unloaded (once module unloading is actually
    implemented). ``HPyGlobal`` provides indirection to isolate subinterpreters.
    Implementation wise, ``HPyGlobal`` will usually contain an index to a table
    with Python objects stored in the interpreter state.

  * On CPython without subinterpreters support, ``HPy``, ``HPyGlobal``,
    and ``HPyField`` are implemented as ``PyObject *``.

  * On CPython with subinterpreters support, ``HPyGlobal`` will be implemented
    by an indirection through the interpreter state. Note that thanks to the HPy
    design, switching between this and the more efficient implementation without
    subinterpreter support will not require rebuilding of the extension (in HPy
    universal mode), nor rebuilding of CPython.

.. note:: If you write a custom type having ``HPyField`` s, you **MUST** also
    write a ``tp_traverse`` slot. Note that this is different than the old
    ``Python.h`` API, where you need ``tp_traverse`` only under certain
    conditions. See the next section for more details.

.. note:: The contract of ``tp_traverse`` is that it must visit all the
    ``HPyFields`` contained within given struct, or more precisely *owned* by
    given Python object (in the sense of the *owner* argument to
    ``HPyField_Store``), and nothing more, nothing less. Some Python
    implementations may choose to not call the provided ``tp_traverse`` if they
    know how to visit all the ``HPyFields`` by other means (for example, when
    they track them internally already). The debug mode will check this
    contract.

``tp_traverse``, ``tp_clear``, ``Py_TPFLAGS_HAVE_GC``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let's quote the ``Python.h`` documentation about `GC support
<https://docs.python.org/3/c-api/gcsupport.html>`_

  Python's support for detecting and collecting garbage which involves
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Generally speaking, if you have one or more ``PyObject *`` fields in the old
``Python.h``, you must provide a ``tp_dealloc`` slot where you ``Py_DECREF`` all
of them. In HPy this is not needed and will be handled automatically by the
system.

In particular, when running on top of CPython, HPy will automatically provide
a ``tp_dealloc`` which decrefs all the fields listed by ``tp_traverse``.


Direct C API to HPy Mappings
----------------------------

In many cases, migrating to HPy is as easy as just replacing a certain C API
function by the appropriate HPy API function. The table :ref:`table-mapping` a
mapping between C API and HPy API functions. You can just apply this mapping
without being cautious because this is a generated mapping which HPy also uses
internally in the :term:`CPython ABI` mode.

..  _table-mapping:

.. table:: Safe API function mapping
    :widths: auto

    ================================================ ================================================
    C API function                                   HPy API function
    ================================================ ================================================
    ``PyLong_FromLong``                              :c:func:`HPyLong_FromLong`
    ``PyLong_FromUnsignedLong``                      :c:func:`HPyLong_FromUnsignedLong`
    ``PyLong_FromLongLong``                          :c:func:`HPyLong_FromLongLong`
    ``PyLong_FromUnsignedLongLong``                  :c:func:`HPyLong_FromUnsignedLongLong`
    ``PyLong_FromSize_t``                            :c:func:`HPyLong_FromSize_t`
    ``PyLong_FromSsize_t``                           :c:func:`HPyLong_FromSsize_t`
    ``PyLong_AsLong``                                :c:func:`HPyLong_AsLong`
    ``PyLong_AsUnsignedLong``                        :c:func:`HPyLong_AsUnsignedLong`
    ``PyLong_AsUnsignedLongMask``                    :c:func:`HPyLong_AsUnsignedLongMask`
    ``PyLong_AsLongLong``                            :c:func:`HPyLong_AsLongLong`
    ``PyLong_AsUnsignedLongLong``                    :c:func:`HPyLong_AsUnsignedLongLong`
    ``PyLong_AsUnsignedLongLongMask``                :c:func:`HPyLong_AsUnsignedLongLongMask`
    ``PyLong_AsSize_t``                              :c:func:`HPyLong_AsSize_t`
    ``PyLong_AsSsize_t``                             :c:func:`HPyLong_AsSsize_t`
    ``PyLong_AsVoidPtr``                             :c:func:`HPyLong_AsVoidPtr`
    ``PyLong_AsDouble``                              :c:func:`HPyLong_AsDouble`
    ``PyFloat_FromDouble``                           :c:func:`HPyFloat_FromDouble`
    ``PyFloat_AsDouble``                             :c:func:`HPyFloat_AsDouble`
    ``PyBool_FromLong``                              :c:func:`HPyBool_FromLong`
    ``PyObject_Length``                              :c:func:`HPy_Length`
    ``PyNumber_Check``                               :c:func:`HPyNumber_Check`
    ``PyNumber_Add``                                 :c:func:`HPy_Add`
    ``PyNumber_Subtract``                            :c:func:`HPy_Subtract`
    ``PyNumber_Multiply``                            :c:func:`HPy_Multiply`
    ``PyNumber_MatrixMultiply``                      :c:func:`HPy_MatrixMultiply`
    ``PyNumber_FloorDivide``                         :c:func:`HPy_FloorDivide`
    ``PyNumber_TrueDivide``                          :c:func:`HPy_TrueDivide`
    ``PyNumber_Remainder``                           :c:func:`HPy_Remainder`
    ``PyNumber_Divmod``                              :c:func:`HPy_Divmod`
    ``PyNumber_Power``                               :c:func:`HPy_Power`
    ``PyNumber_Negative``                            :c:func:`HPy_Negative`
    ``PyNumber_Positive``                            :c:func:`HPy_Positive`
    ``PyNumber_Absolute``                            :c:func:`HPy_Absolute`
    ``PyNumber_Invert``                              :c:func:`HPy_Invert`
    ``PyNumber_Lshift``                              :c:func:`HPy_Lshift`
    ``PyNumber_Rshift``                              :c:func:`HPy_Rshift`
    ``PyNumber_And``                                 :c:func:`HPy_And`
    ``PyNumber_Xor``                                 :c:func:`HPy_Xor`
    ``PyNumber_Or``                                  :c:func:`HPy_Or`
    ``PyNumber_Index``                               :c:func:`HPy_Index`
    ``PyNumber_Long``                                :c:func:`HPy_Long`
    ``PyNumber_Float``                               :c:func:`HPy_Float`
    ``PyNumber_InPlaceAdd``                          :c:func:`HPy_InPlaceAdd`
    ``PyNumber_InPlaceSubtract``                     :c:func:`HPy_InPlaceSubtract`
    ``PyNumber_InPlaceMultiply``                     :c:func:`HPy_InPlaceMultiply`
    ``PyNumber_InPlaceMatrixMultiply``               :c:func:`HPy_InPlaceMatrixMultiply`
    ``PyNumber_InPlaceFloorDivide``                  :c:func:`HPy_InPlaceFloorDivide`
    ``PyNumber_InPlaceTrueDivide``                   :c:func:`HPy_InPlaceTrueDivide`
    ``PyNumber_InPlaceRemainder``                    :c:func:`HPy_InPlaceRemainder`
    ``PyNumber_InPlacePower``                        :c:func:`HPy_InPlacePower`
    ``PyNumber_InPlaceLshift``                       :c:func:`HPy_InPlaceLshift`
    ``PyNumber_InPlaceRshift``                       :c:func:`HPy_InPlaceRshift`
    ``PyNumber_InPlaceAnd``                          :c:func:`HPy_InPlaceAnd`
    ``PyNumber_InPlaceXor``                          :c:func:`HPy_InPlaceXor`
    ``PyNumber_InPlaceOr``                           :c:func:`HPy_InPlaceOr`
    ``PyCallable_Check``                             :c:func:`HPyCallable_Check`
    ``PyErr_SetString``                              :c:func:`HPyErr_SetString`
    ``PyErr_SetObject``                              :c:func:`HPyErr_SetObject`
    ``PyErr_SetFromErrnoWithFilename``               :c:func:`HPyErr_SetFromErrnoWithFilename`
    ``PyErr_SetFromErrnoWithFilenameObjects``        :c:func:`HPyErr_SetFromErrnoWithFilenameObjects`
    ``PyErr_ExceptionMatches``                       :c:func:`HPyErr_ExceptionMatches`
    ``PyErr_NoMemory``                               :c:func:`HPyErr_NoMemory`
    ``PyErr_Clear``                                  :c:func:`HPyErr_Clear`
    ``PyErr_NewException``                           :c:func:`HPyErr_NewException`
    ``PyErr_NewExceptionWithDoc``                    :c:func:`HPyErr_NewExceptionWithDoc`
    ``PyErr_WarnEx``                                 :c:func:`HPyErr_WarnEx`
    ``PyErr_WriteUnraisable``                        :c:func:`HPyErr_WriteUnraisable`
    ``PyObject_IsTrue``                              :c:func:`HPy_IsTrue`
    ``PyObject_GetAttr``                             :c:func:`HPy_GetAttr`
    ``PyObject_GetAttrString``                       :c:func:`HPy_GetAttr_s`
    ``PyObject_HasAttr``                             :c:func:`HPy_HasAttr`
    ``PyObject_HasAttrString``                       :c:func:`HPy_HasAttr_s`
    ``PyObject_SetAttr``                             :c:func:`HPy_SetAttr`
    ``PyObject_SetAttrString``                       :c:func:`HPy_SetAttr_s`
    ``PyObject_GetItem``                             :c:func:`HPy_GetItem`
    ``PySequence_Contains``                          :c:func:`HPy_Contains`
    ``PyObject_SetItem``                             :c:func:`HPy_SetItem`
    ``PyObject_DelItem``                             :c:func:`HPy_DelItem`
    ``PyObject_Type``                                :c:func:`HPy_Type`
    ``PyObject_Repr``                                :c:func:`HPy_Repr`
    ``PyObject_Str``                                 :c:func:`HPy_Str`
    ``PyObject_ASCII``                               :c:func:`HPy_ASCII`
    ``PyObject_Bytes``                               :c:func:`HPy_Bytes`
    ``PyObject_RichCompare``                         :c:func:`HPy_RichCompare`
    ``PyObject_RichCompareBool``                     :c:func:`HPy_RichCompareBool`
    ``PyObject_Hash``                                :c:func:`HPy_Hash`
    ``PyBytes_Check``                                :c:func:`HPyBytes_Check`
    ``PyBytes_Size``                                 :c:func:`HPyBytes_Size`
    ``PyBytes_GET_SIZE``                             :c:func:`HPyBytes_GET_SIZE`
    ``PyBytes_AsString``                             :c:func:`HPyBytes_AsString`
    ``PyBytes_AS_STRING``                            :c:func:`HPyBytes_AS_STRING`
    ``PyBytes_FromString``                           :c:func:`HPyBytes_FromString`
    ``PyUnicode_FromString``                         :c:func:`HPyUnicode_FromString`
    ``PyUnicode_Check``                              :c:func:`HPyUnicode_Check`
    ``PyUnicode_AsASCIIString``                      :c:func:`HPyUnicode_AsASCIIString`
    ``PyUnicode_AsLatin1String``                     :c:func:`HPyUnicode_AsLatin1String`
    ``PyUnicode_AsUTF8String``                       :c:func:`HPyUnicode_AsUTF8String`
    ``PyUnicode_AsUTF8AndSize``                      :c:func:`HPyUnicode_AsUTF8AndSize`
    ``PyUnicode_FromWideChar``                       :c:func:`HPyUnicode_FromWideChar`
    ``PyUnicode_DecodeFSDefault``                    :c:func:`HPyUnicode_DecodeFSDefault`
    ``PyUnicode_DecodeFSDefaultAndSize``             :c:func:`HPyUnicode_DecodeFSDefaultAndSize`
    ``PyUnicode_EncodeFSDefault``                    :c:func:`HPyUnicode_EncodeFSDefault`
    ``PyUnicode_ReadChar``                           :c:func:`HPyUnicode_ReadChar`
    ``PyUnicode_DecodeASCII``                        :c:func:`HPyUnicode_DecodeASCII`
    ``PyUnicode_DecodeLatin1``                       :c:func:`HPyUnicode_DecodeLatin1`
    ``PyList_Check``                                 :c:func:`HPyList_Check`
    ``PyList_New``                                   :c:func:`HPyList_New`
    ``PyList_Append``                                :c:func:`HPyList_Append`
    ``PyDict_Check``                                 :c:func:`HPyDict_Check`
    ``PyDict_New``                                   :c:func:`HPyDict_New`
    ``PyTuple_Check``                                :c:func:`HPyTuple_Check`
    ``PyImport_ImportModule``                        :c:func:`HPyImport_ImportModule`
    ``PyCapsule_IsValid``                            :c:func:`HPyCapsule_IsValid`
    ``PyEval_RestoreThread``                         :c:func:`HPy_ReenterPythonExecution`
    ``PyEval_SaveThread``                            :c:func:`HPy_LeavePythonExecution`
    ================================================ ================================================


.. note: There are, of course, also cases where it is not possible to map directly and safely from a C API function (or concept) to an HPy API function (or concept).

Reference Counting ``Py_INCREF`` and ``Py_DECREF``
--------------------------------------------------

The equivalents of ``Py_INCREF`` and ``Py_DECREF`` are essentially
:c:func:`HPy_Dup` and :c:func:`HPy_Close`, respectively. The main difference is
that :c:func:`HPy_Dup` gives you a *new handle* to the same object which means
that the two handles may be different if comparing them with ``memcmp`` but
still reference the same object. As a consequence, you may close a handle only
once, i.e., you cannot call :c:func:`HPy_Close` twice on the same ``HPy``
handle. For examples, see also sections :ref:`api:handles` and :ref:`api:handles
vs ``pyobject *```

Call Functions ``PyObject_Call`` and ``PyObject_CallObject``
------------------------------------------------------------

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


PyModule_AddObject
------------------

``PyModule_AddObject`` is replaced with a regular :c:func:`HPy_SetAttr_s`. There
is no ``HPyModule_AddObject`` function because it has an unusual refcount
behavior (stealing a reference but only when it returns ``0``).

Deallocator Slot ``Py_tp_dealloc``
----------------------------------

``Py_tp_dealloc`` essentially becomes ``HPy_tp_destroy``. The name intentionally
differs because there are major differences: while the slot function of
``Py_tp_dealloc`` receives the full object (which makes it possible to resurrect
it) and while there are no restrictions on what you may call in the C API
deallocator, you must not do that in HPy's deallocator.

The two major restrictions apply to the slot function of ``HPy_tp_destroy``:

1. The function must be **thread-safe**.
2. The function **must not** call into the interpreter.

The idea is, that ``HPy_tp_destroy`` just releases native resources (e.g. by
using C lib's ``free`` function). Therefore, it does only receive a pointer to
the object's native data (and not a handle to the object) and it does not
receive an ``HPyContext`` pointer argument.

For the time being, HPy will support the ``HPy_tp_finalize`` slot where those
tight restrictions do not apply at the (significant) cost of performance.

Special Slots ``Py_tp_methods``, ``Py_tp_members``, and ``Py_tp_getset``
------------------------------------------------------------------------

There is not direct replacement for C API slots ``Py_tp_methods``,
``Py_tp_members``, and ``Py_tp_getset`` because they are no longer needed.
Methods, members, and get/set descriptors are specified *flatly* together with
the other slots, using the standard mechanisms of :c:macro:`HPyDef_METH`,
:c:macro:`HPyDef_MEMBER`, and :c:macro:`HPyDef_GETSET`. The resulting ``HPyDef``
structures are then accumulated in :c:member:`HPyType_Spec.defines`.

Creating Lists and Tuples
-------------------------

The C API way of creating lists and tuples is to create an empty list or tuple
object using ``PyList_New(n)`` or ``PyTuple_New(n)``, respectively, and then to
fill the empty object using ``PyList_SetItem / PyList_SET_ITEM`` or
``PyTuple_SetItem / PyTuple_SET_ITEM``, respectively.

This is in particular problematic for tuples because they are actually
immutable. HPy goes a different way and provides a dedicated *builder* API to
avoid the (temporary) inconsitent state during object initialization.

Long story short, doing the same in HPy with builders is still very simple and
straight forward. Following an example for creating a list:

.. code-block:: c

    PyObject *list = PyList_New(5);
    if (list == NULL)
        return NULL; /* error */
    PyList_SET_ITEM(list, 0, item0);
    PyList_SET_ITEM(list, 1, item0);
    ...
    PyList_SET_ITEM(list, 4, item0);
    /* now 'list' is ready to use */

becomes

.. code-block:: c

    HPyListBuilder builder = HPyListBuilder_New(ctx, 5);
    HPyListBuilder_Set(ctx, builder, 0, h_item0);
    HPyListBuilder_Set(ctx, builder, 1, h_item1);
    ...
    HPyListBuilder_Set(ctx, builder, 4, h_item4);
    HPy h_list = HPyListBuilder_Build(ctx, builder);
    if (HPy_IsNull(h_list))
        return HPy_NULL; /* error */

.. note:: In contrast to ``PyList_SetItem``, ``PyList_SET_ITEM``,
   ``PyTuple_SetItem``, and ``PyTuple_SET_ITEM``, the builder functions
   :c:func:`HPyListBuilder_Set` and :c:func:`HPyTupleBuilder_Set` are **NOT**
   stealing references. It is necessary to close the passed item handles (e.g.
   ``h_item0`` in the above example) if they are no longer needed.

If an error occurs during building the list or tuple, it is necessary to call
:c:func:`HPyListBuilder_Cancel` or :c:func:`HPyTupleBuilder_Cancel`,
respectively, to avoid memory leaks.

For details, see the API reference documentation :doc:`api-reference/builder`.

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

