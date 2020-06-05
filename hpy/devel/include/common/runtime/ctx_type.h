#ifndef HPY_COMMON_RUNTIME_CTX_TYPE_H
#define HPY_COMMON_RUNTIME_CTX_TYPE_H

#include <Python.h>
#include "hpy.h"
#include "common/type.h"

_HPy_HIDDEN void* ctx_Cast(HPyContext ctx, HPy h);
_HPy_HIDDEN HPy ctx_Type_FromSpec(HPyContext ctx, HPyType_Spec *hpyspec);
_HPy_HIDDEN HPy ctx_New(HPyContext ctx, HPy h_type, void **data);

#ifdef HPY_UNIVERSAL_ABI
// this function is used by hpy/universal/src/ctx_module.c, there is no need
// to export it in the CPython ABI mode
_HPy_HIDDEN PyMethodDef *create_method_defs(HPyMeth *hpymethods[]);
#endif

#endif /* HPY_COMMON_RUNTIME_CTX_TYPE_H */
