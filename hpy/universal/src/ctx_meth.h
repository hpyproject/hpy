#include "hpy.h"
#include "api.h"

HPyAPI_IMPL void
ctx_CallRealFunctionFromTrampoline(HPyContext *ctx, HPyFunc_Signature sig,
                                   void* (*func)(), void *args);
