#ifndef HPY_COMMON_RUNTIME_CTX_OBJECT_H
#define HPY_COMMON_RUNTIME_CTX_OBJECT_H

#include <Python.h>
#include "hpy.h"

_HPy_HIDDEN void ctx_Dump(HPyContext *ctx, HPy h);
_HPy_HIDDEN int ctx_TypeCheck(HPyContext *ctx, HPy h_obj, HPy h_type);
_HPy_HIDDEN int ctx_Is(HPyContext *ctx, HPy h_obj, HPy h_other);
_HPy_HIDDEN HPy ctx_GetItem_i(HPyContext *ctx, HPy obj, HPy_ssize_t idx);
_HPy_HIDDEN HPy ctx_GetItem_s(HPyContext *ctx, HPy obj, const char *key);
_HPy_HIDDEN int ctx_SetItem_i(HPyContext *ctx, HPy obj, HPy_ssize_t idx, HPy value);
_HPy_HIDDEN int ctx_SetItem_s(HPyContext *ctx, HPy obj, const char *key, HPy value);

#endif /* HPY_COMMON_RUNTIME_CTX_OBJECT_H */
