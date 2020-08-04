#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE void
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, HPyFunc_Signature sig,
                                   void *func, void *args);
