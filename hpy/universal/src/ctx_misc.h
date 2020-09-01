#ifndef HPY_CTX_MISC_H
#define HPY_CTX_MISC_H

#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE HPy ctx_FromPyObject(HPyContext ctx, cpy_PyObject *obj);
HPyAPI_STORAGE cpy_PyObject *ctx_AsPyObject(HPyContext ctx, HPy h);
HPyAPI_STORAGE void ctx_Close(HPyContext ctx, HPy h);
HPyAPI_STORAGE HPy ctx_Dup(HPyContext ctx, HPy h);
HPyAPI_STORAGE void ctx_FatalError(HPyContext ctx, const char *message);

#endif /* HPY_CTX_MISC_H */
