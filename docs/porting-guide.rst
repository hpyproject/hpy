Porting guide
=============

xxx
---------------------

`PyModule_AddObject()`: replace with a regular `HPy_SetAttr_s()`.  There is no `HPyModule_AddObject()` because it has an unusual refcount behaviour (stealing a reference but only when it returns 0).

