// This is for non-CPython implementations!
//
// If you want to bundle the debug mode into your own version of
// hpy.universal, make sure to compile this file and NOT debug_ctx_cpython.c

#include "debug_internal.h"

void debug_ctx_CallRealFunctionFromTrampoline(HPyContext ctx,
                                              HPyFunc_Signature sig,
                                              void *func, void *args)
{
    HPyContext original_ctx = get_info(ctx)->original_ctx;
    HPy_FatalError(original_ctx,
                   "Something is very wrong! _HPy_CallRealFunctionFromTrampoline() "
                   "should be used only by the CPython version of hpy.universal");
}
