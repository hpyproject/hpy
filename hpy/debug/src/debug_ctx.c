#include <string.h>
#include <stdio.h>
#include "debug_internal.h"
#include "autogen_debug_ctx_init.h"

static struct _HPyContext_s g_debug_ctx = {
    .name = "HPy Debug Mode ABI",
    ._private = NULL,
    .ctx_version = 1,
};

void debug_ctx_Close(HPyContext ctx, DHPy dh)
{
    UHPy uh = DHPy_unwrap(dh);
    DHPy_close(ctx, dh);
    HPy_Close(get_info(ctx)->uctx, uh);
}


// NOTE: at the moment this function assumes that uctx is always the
// same. If/when we migrate to a system in which we can have multiple
// independent contexts, this function should ensure to create a different
// debug wrapper for each of them.
static void debug_ctx_init(HPyContext dctx, HPyContext uctx)
{
    if (dctx->_private != NULL) {
        // already initialized
        assert(get_info(dctx)->uctx == uctx); // sanity check
        return;
    }
    // initialize debug_info
    // XXX: currently we never free this malloc
    HPyDebugInfo *info = malloc(sizeof(HPyDebugInfo));
    info->magic_number = HPY_DEBUG_MAGIC;
    info->uctx = uctx;
    info->open_handles = NULL;
    info->closed_handles = NULL;
    dctx->_private = info;

    debug_ctx_init_fields(dctx, uctx);
}

HPyContext hpy_debug_get_ctx(HPyContext uctx)
{
    HPyContext dctx = &g_debug_ctx;
    debug_ctx_init(dctx, uctx);
    return dctx;
}

// this function is supposed to be called from gdb: it tries to determine
// whether a handle is universal or debug by looking at the last bit
extern struct _HPyContext_s g_universal_ctx;
__attribute__((unused)) static void hpy_magic_dump(HPy h)
{
    int universal = h._i & 1;
    if (universal)
        fprintf(stderr, "\nUniversal handle\n");
    else
        fprintf(stderr, "\nDebug handle\n");

    fprintf(stderr, "raw value: %lx (%ld)\n", h._i, h._i);
    if (universal)
        _HPy_Dump(&g_universal_ctx, h);
    else {
        DebugHandle *dh = as_DebugHandle(h);
        fprintf(stderr, "dh->uh: %lx\n", dh->uh._i);
        _HPy_Dump(&g_universal_ctx, dh->uh);
    }
}
