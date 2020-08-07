Porting guide
=============

xxx
---------------------

`PyModule_AddObject()`: replace with a regular `HPy_SetAttr_s()`.  There is no `HPyModule_AddObject()` because it has an unusual refcount behaviour (stealing a reference but only when it returns 0).

Py_tp_dealloc becomes HPy_tp_destroy.  We changed the name a little bit
because only "lightweight" destructors are supported.  Use tp_finalize if
you really need to do things with the context or with the handle of the
object.
