Porting guide
=============

xxx
---------------------

`PyModule_AddObject()`: replace with a regular `HPy_SetAttr_s()`.  There is no `HPyModule_AddObject()` because it has an unusual refcount behaviour (stealing a reference but only when it returns 0).

Py_tp_dealloc becomes HPy_tp_destroy.  We changed the name a little bit
because only "lightweight" destructors are supported.  Use tp_finalize if
you really need to do things with the context or with the handle of the
object.

PyList_New(5)/PyList_SET_ITEM() becomes::

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
