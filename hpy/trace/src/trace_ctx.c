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

static HPy create_trace_func_args(HPyContext *uctx, int id)
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
    HPy_FatalError(uctx, "could not create arguments for user trace function");
    return HPy_NULL;
}

static inline int64_t diff_ns(struct timespec *start, struct timespec *end)
{
    return ((int64_t)end->tv_sec - (int64_t)start->tv_sec) * (int64_t)1000000000
            + ((int64_t)end->tv_nsec - (int64_t)start->tv_nsec);
}

HPyTraceInfo *hpy_trace_on_enter(HPyContext *tctx, int id)
{
    HPyTraceInfo *tctx_info = get_info(tctx);
    HPyContext *uctx = tctx_info->uctx;
    HPy args, res;
    tctx_info->call_counts[id]++;
    if(!HPy_IsNull(tctx_info->on_enter_func)) {
        args = create_trace_func_args(uctx, id);
        res = HPy_CallTupleDict(
                uctx, tctx_info->on_enter_func, args, HPy_NULL);
        HPy_Close(uctx, args);
        if (HPy_IsNull(res)) {
            HPy_FatalError(uctx,
                    "error when executing on-enter trace function");
        }
    }
    return tctx_info;
}

void hpy_trace_on_exit(HPyTraceInfo *info, int id, int cr,
        struct timespec *_ts_start, struct timespec *_ts_end)
{
    HPyContext *uctx = info->uctx;
    HPy args, res;
    if (cr)
        HPy_FatalError(uctx, "could not get monotonic clock");
    int64_t duration = diff_ns(_ts_start, _ts_end);
    assert(duration >= 0);
    info->durations[id] += duration;
    if(!HPy_IsNull(info->on_exit_func)) {
        args = create_trace_func_args(uctx, id);
        res = HPy_CallTupleDict(uctx, info->on_exit_func, args, HPy_NULL);
        HPy_Close(uctx, args);
        if (HPy_IsNull(res)) {
            HPy_FatalError(uctx,
                    "error when executing on-exit trace function");
        }
    }
}

