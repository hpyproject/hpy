#include "trace_internal.h"
#include "autogen_trace_ctx_init.h"

static struct _HPyContext_s g_trace_ctx = {
    .name = "HPy Trace Mode ABI",
    ._private = NULL,
    .abi_version = HPY_ABI_VERSION,
};

// NOTE: at the moment this function assumes that uctx is always the
// same. If/when we migrate to a system in which we can have multiple
// independent contexts, this function should ensure to create a different
// debug wrapper for each of them.
int hpy_trace_ctx_init(HPyContext *tctx, HPyContext *uctx)
{
    if (tctx->_private != NULL) {
        // already initialized
        assert(get_info(tctx)->uctx == uctx); // sanity check
        return 0;
    }
    // initialize trace_info
    // XXX: currently we never free this malloc
    HPyTraceInfo *info = malloc(sizeof(HPyTraceInfo));
    if (info == NULL) {
        HPyErr_NoMemory(uctx);
        return -1;
    }
    trace_ctx_init_info(info, uctx);
    tctx->_private = info;
    trace_ctx_init_fields(tctx, uctx);
    return 0;
}

int hpy_trace_ctx_free(HPyContext *tctx)
{
    trace_ctx_free_info(get_info(tctx));
    return 0;
}

HPyContext * hpy_trace_get_ctx(HPyContext *uctx)
{
    HPyContext *tctx = &g_trace_ctx;
    if (uctx == tctx) {
        HPy_FatalError(uctx, "hpy_trace_get_ctx: expected an universal ctx, "
                             "got a trace ctx");
    }
    if (hpy_trace_ctx_init(tctx, uctx) < 0)
        return NULL;
    return tctx;
}

void hpy_trace_set_ctx(HPyContext *tctx)
{
    g_trace_ctx = *tctx;
}

HPy hpy_trace_create_func_args(HPyContext *uctx, int id)
{
    HPy h_name = HPyUnicode_FromString(uctx, hpy_trace_get_func_name(id));
    if (HPy_IsNull(h_name))
        goto fail;
    HPy h_args = HPyTuple_FromArray(uctx, &h_name, 1);
    if (HPy_IsNull(h_args))
        goto fail;
    HPy_Close(uctx, h_name);
    return h_args;
fail:
    HPy_FatalError(uctx, "could not ");
}

void hpy_trace_on_enter(HPyTraceInfo *info, HPyContext *uctx, HPy args)
{
    HPy res = HPy_CallTupleDict(uctx, info->on_enter_func, args, HPy_NULL);
    if (HPy_IsNull(res))
        HPy_FatalError(uctx, "error when executing on-enter trace function");
}

void hpy_trace_on_exit(HPyTraceInfo *info, HPyContext *uctx, HPy args)
{
    HPy res = HPy_CallTupleDict(uctx, info->on_exit_func, args, HPy_NULL);
    if (HPy_IsNull(res))
        HPy_FatalError(uctx, "error when executing on-exit trace function");
}

