#ifndef HPY_CTX_TYPE_H
#define HPY_CTX_TYPE_H

#include "hpy.h"
#include "api.h"

HPyAPI_STORAGE void* ctx_Cast(HPyContext ctx, HPy h);
HPyAPI_STORAGE HPy ctx_Type_FromSpec(HPyContext ctx, HPyType_Spec *hpyspec);
HPyAPI_STORAGE HPy ctx_New(HPyContext ctx, HPy h_type, void **data);

#endif /* HPY_CTX_TYPE_H */
