#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE void
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, HPyFunc_Signature sig,
                                   void *func, void *args);

HPyAPI_STORAGE void
ctx_CallDestroyAndThenDealloc(HPyContext ctx, void *func, PyObject *self);
