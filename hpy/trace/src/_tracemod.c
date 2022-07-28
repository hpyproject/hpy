// Python-level interface for the _trace module. Written in HPy itself, the
// idea is that it should be reusable by other implementations

// NOTE: hpy.trace._trace is loaded using the UNIVERSAL ctx. To make it
// clearer, we will use "uctx" and "tctx" to distinguish them.

#include "hpy.h"
#include "trace_internal.h"

#define IS_EMPTY(_s) ((_s)[0] == '\0')

HPyDef_METH(durations, "durations", durations_impl, HPyFunc_NOARGS)
static HPy durations_impl(HPyContext *uctx, HPy self)
{
    HPyContext *tctx = hpy_trace_get_ctx(uctx);
    HPyTraceInfo *info = get_info(tctx);
    HPyTracker ht = HPyTracker_New(uctx, hpy_trace_get_nfunc());
    HPy res = HPyDict_New(uctx);
    const char *func_name;
    for (int i=0; (func_name = hpy_trace_get_func_name(i)); i++)
    {
        /* skip empty names; those indices denote a context handle */
        if (!IS_EMPTY(func_name))
        {
            HPy value = HPyLong_FromLong(uctx, info->durations[i]);
            HPyTracker_Add(uctx, ht, value);
            if (HPy_IsNull(value))
                goto fail;
            if (HPy_SetItem_s(uctx, res, func_name, value) < 0)
                goto fail;
        }
    }
    HPyTracker_Close(uctx, ht);
    return res;
fail:
    HPyTracker_Close(uctx, ht);
    return HPy_NULL;
}

HPyDef_METH(call_counts, "call_counts", call_counts_impl, HPyFunc_NOARGS)
static HPy call_counts_impl(HPyContext *uctx, HPy self)
{
    HPyContext *tctx = hpy_trace_get_ctx(uctx);
    HPyTraceInfo *info = get_info(tctx);
    HPyTracker ht = HPyTracker_New(uctx, hpy_trace_get_nfunc());
    HPy res = HPyDict_New(uctx);
    const char *func_name;
    for (int i=0; (func_name = hpy_trace_get_func_name(i)); i++)
    {
        /* skip empty names; those indices denote a context handle */
        if (!IS_EMPTY(func_name))
        {
            HPy value = HPyLong_FromUnsignedLongLong(uctx,
                    (unsigned long long)info->call_counts[i]);
            HPyTracker_Add(uctx, ht, value);
            if (HPy_IsNull(value))
                goto fail;
            if (HPy_SetItem_s(uctx, res, func_name, value) < 0)
                goto fail;
        }
    }
    HPyTracker_Close(uctx, ht);
    return res;
fail:
    HPyTracker_Close(uctx, ht);
    return HPy_NULL;
}

/* ~~~~~~ definition of the module hpy.trace._trace ~~~~~~~ */

static HPyDef *module_defines[] = {
    &durations,
    &call_counts,
    NULL
};

static HPyModuleDef moduledef = {
    .name = "hpy.trace._trace",
    .doc = "HPy trace mode",
    .size = -1,
    .defines = module_defines
};


HPy_MODINIT(_trace)
static HPy init__trace_impl(HPyContext *uctx)
{
    return HPyModule_Create(uctx, &moduledef);
}
