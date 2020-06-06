#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE cpy_PyObject *
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, cpy_PyObject *self,
                                   cpy_PyObject *args, cpy_PyObject *kw,
                                   void *func, HPyMeth_Signature sig);
