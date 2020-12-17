#include "hpy_debug.h"
#include "debug_ctx.h"
#include <stdio.h>
#include <string.h>

static HPyDebugInfo debug_info = {
    .magic_number = HPY_DEBUG_MAGIC,
    .original_ctx = NULL,
};

static struct _HPyContext_s debug_ctx = {
    .name = NULL,
};

static HPy dbg_Add(HPyContext ctx, HPy a, HPy b)
{
    printf("dbg_Add...\n");
    HPyDebugInfo *info = get_info(ctx);
    return HPy_Add(info->original_ctx, a, b);
}

void debug_ctx_init(HPyContext original_ctx)
{
    if (debug_ctx.name) {
        // already initialized
        assert(get_info(debug_ctx)->original_ctx == original_ctx); // sanity check
        return;
    }

    // initialize debug_info
    debug_info.original_ctx = original_ctx;

    // initialize debug_ctx: eventually we will autogen a static initializer
    // for debug_ctx. For now, just copy&fix
    memcpy(&debug_ctx, original_ctx, sizeof(struct _HPyContext_s));
    debug_ctx.name = "HPy Debug Mode ABI";
    debug_ctx._private = &debug_info;
    debug_ctx.ctx_Add = dbg_Add;
}

HPyContext hpy_debug_get_ctx(HPyContext original_ctx)
{
    debug_ctx_init(original_ctx);
    return &debug_ctx;
}
