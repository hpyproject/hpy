#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE cpy_PyObject *
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, HPyFunc_Signature sig,
                                   void *func, void *args);
