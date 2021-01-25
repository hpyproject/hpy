#include <string.h>
#include <stdio.h>
#include "debug_internal.h"
#include "autogen_debug_ctx_init.h"

static struct _HPyContext_s g_debug_ctx = {
    .name = "HPy Debug Mode ABI",
    ._private = NULL,
    .ctx_version = 1,
};

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
    info->current_generation = 0;
    info->open_handles = NULL;
    //info->closed_handles = NULL;
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

/* ~~~~~~~~~~ manually written wrappers ~~~~~~~~~~ */

void debug_ctx_Close(HPyContext dctx, DHPy dh)
{
    UHPy uh = DHPy_unwrap(dh);
    DHPy_close(dctx, dh);
    HPy_Close(get_info(dctx)->uctx, uh);
}

DHPy debug_ctx_Tuple_FromArray(HPyContext dctx, DHPy dh_items[], HPy_ssize_t n)
{
    // NOTE: replace VLAs with alloca() once issue #157 is fixed
    UHPy uh_items[n];
    for(int i=0; i<n; i++) {
        uh_items[i] = DHPy_unwrap(dh_items[i]);
    }
    return DHPy_wrap(dctx, HPyTuple_FromArray(get_info(dctx)->uctx, uh_items, n));
}

DHPy debug_ctx_Type_GenericNew(HPyContext dctx, DHPy type, DHPy *args, HPy_ssize_t nargs,
                               DHPy kw)
{
    abort();
}
