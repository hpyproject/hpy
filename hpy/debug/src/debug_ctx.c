#include <string.h>
#include <stdio.h>
#include "debug_internal.h"
#include "autogen_debug_ctx.h"

static HPyDebugInfo debug_info = {
    .magic_number = HPY_DEBUG_MAGIC,
    .uctx = NULL,
};

// NOTE: at the moment this function assumes that uctx is always the
// same. If/when we migrate to a system in which we can have multiple
// independent contexts, this function should ensure to create a different
// debug wrapper for each of them.
static void debug_ctx_init(HPyContext uctx)
{
    if (g_debug_ctx._private != NULL) {
        // already initialized
        assert(get_info(&g_debug_ctx)->uctx == uctx); // sanity check
        return;
    }

    // initialize debug_info
    debug_info.uctx = uctx;
    debug_info.open_handles = NULL;
    debug_info.closed_handles = NULL;
    g_debug_ctx._private = &debug_info;

    // initialze ctx->h_None, etc.
    debug_init_prebuilt_handles(&g_debug_ctx, uctx);
}

HPyContext hpy_debug_get_ctx(HPyContext uctx)
{
    debug_ctx_init(uctx);
    return &g_debug_ctx;
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
