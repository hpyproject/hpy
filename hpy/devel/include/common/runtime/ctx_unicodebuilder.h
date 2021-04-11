#ifndef HPY_COMMON_RUNTIME_UNICODEBUILDER_H
#define HPY_COMMON_RUNTIME_UNICODEBUILDER_H

#include <Python.h>
#include "hpy.h"
#include "common/hpytype.h"


_HPy_HIDDEN HPyUnicodeBuilder ctx_UnicodeBuilder_New(HPyContext *ctx);
_HPy_HIDDEN void ctx_UnicodeBuilder_Append(HPyContext *ctx, HPyUnicodeBuilder builder,
                                          HPy h_item);
_HPy_HIDDEN HPy ctx_UnicodeBuilder_Build(HPyContext *ctx, HPyUnicodeBuilder builder);
_HPy_HIDDEN void ctx_UnicodeBuilder_Cancel(HPyContext *ctx, HPyUnicodeBuilder builder);


#endif /* HPY_COMMON_RUNTIME_UNICODEBUILDER_H */
