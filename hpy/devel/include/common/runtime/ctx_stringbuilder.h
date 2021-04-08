#ifndef HPY_COMMON_RUNTIME_STRINGBUILDER_H
#define HPY_COMMON_RUNTIME_STRINGBUILDER_H

#include <Python.h>
#include "hpy.h"
#include "common/hpytype.h"


_HPy_HIDDEN HPyStringBuilder ctx_StringBuilder_New(HPyContext *ctx);
_HPy_HIDDEN void ctx_StringBuilder_Append(HPyContext *ctx, HPyStringBuilder builder,
                                          HPy h_item);
_HPy_HIDDEN HPy ctx_StringBuilder_Build(HPyContext *ctx, HPyStringBuilder builder);
_HPy_HIDDEN void ctx_StringBuilder_Cancel(HPyContext *ctx, HPyStringBuilder builder);


#endif /* HPY_COMMON_RUNTIME_STRINGBUILDER_H */
