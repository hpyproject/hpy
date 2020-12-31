#include <string.h>
#include <stdio.h>
#include "debug_internal.h"
#include "autogen_debug_ctx.h"

static HPyDebugInfo debug_info = {
    .magic_number = HPY_DEBUG_MAGIC,
    .original_ctx = NULL,
};


void debug_ctx_CallRealFunctionFromTrampoline(HPyContext ctx,
                                              HPyFunc_Signature sig,
                                              void *func, void *args)
{
    fprintf(stderr,
            "FATAL ERROR! debug_ctx_CallRealFunctionFromTrampoline should never be "
            "called! This probably means that the debug_ctx was not initialized "
            "properly\n");
    abort();
}

// NOTE: at the moment this function assumes that original_ctx is always the
// same. If/when we migrate to a system in which we can have multiple
// independent contexts, this function should ensure to create a different
// debug wrapper for each of them.
static void debug_ctx_init(HPyContext original_ctx)
{
    if (g_debug_ctx._private != NULL) {
        // already initialized
        assert(get_info(&debug_ctx)->original_ctx == original_ctx); // sanity check
        return;
    }

    // initialize debug_info
    debug_info.original_ctx = original_ctx;
    debug_info.open_handles = NULL;
    debug_info.closed_handles = NULL;
    g_debug_ctx._private = &debug_info;

    /* CallRealFunctionFromTrampoline is special, since it is responsible to
       retrieve and pass the appropriate context to the HPy functions on
       CPython. Note that this is used ONLY on CPython, other implementations
       should be able to call HPy functions natively without any need for
       trampolines.

       Quick recap of what happens:

       1. HPy_MODINIT defines a per-module _ctx_for_trampolines

       2. universal.load(..., debug=True) passes g_debug_ctx to MODINIT, which
          stores it in _ctx_for_trampolines

       3. when CPython calls an HPy function, it goes through the trampoline
          which calls CallRealFunctionFromTrampoline

       4. the default implementation retrieves the ctx from
          _ctx_for_trampolines (which will contain either g_universal_ctx or
          g_debug_ctx depending on how the module was loaded) and passes it to
          the HPy func.

       5. So, the default implementation does exactly what we want! Let's just
          copy it from original_ctx
    */
    g_debug_ctx.ctx_CallRealFunctionFromTrampoline = original_ctx->ctx_CallRealFunctionFromTrampoline;
}

HPyContext hpy_debug_get_ctx(HPyContext original_ctx)
{
    debug_ctx_init(original_ctx);
    return &g_debug_ctx;
}
