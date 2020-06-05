#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE struct _object *
ctx_CallRealFunctionFromTrampoline(HPyContext ctx, struct _object *self,
                                   struct _object *args, struct _object *kw,
                                   void *func, HPyMeth_Signature sig);
