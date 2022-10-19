// Python-level interface for the _trace module. Written in HPy itself, the
// idea is that it should be reusable by other implementations

// NOTE: hpy.trace._trace is loaded using the UNIVERSAL ctx. To make it
// clearer, we will use "uctx" and "tctx" to distinguish them.

#include "hpy.h"
#include "trace_internal.h"

static inline int is_empty(const char *s)
{
    return s[0] == '\0';
}

HPyDef_METH(get_durations, "get_durations", get_durations_impl, HPyFunc_NOARGS)
static HPy get_durations_impl(HPyContext *uctx, HPy self)
{
    HPyContext *tctx = hpy_trace_get_ctx(uctx);
    HPyTraceInfo *info = get_info(tctx);
    HPyTracker ht = HPyTracker_New(uctx, hpy_trace_get_nfunc());
    HPy res = HPyDict_New(uctx);
    const char *func_name;
    for (int i=0; (func_name = hpy_trace_get_func_name(i)); i++)
    {
        /* skip empty names; those indices denote a context handle */
        if (!is_empty(func_name))
        {
            HPy value = HPyLong_FromLongLong(uctx,
                    (long long)info->durations[i]);
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
    HPy_Close(uctx, res);
    HPyTracker_Close(uctx, ht);
    return HPy_NULL;
}

HPyDef_METH(get_call_counts, "get_call_counts", get_call_counts_impl, HPyFunc_NOARGS)
static HPy get_call_counts_impl(HPyContext *uctx, HPy self)
{
    HPyContext *tctx = hpy_trace_get_ctx(uctx);
    HPyTraceInfo *info = get_info(tctx);
    HPyTracker ht = HPyTracker_New(uctx, hpy_trace_get_nfunc());
    HPy res = HPyDict_New(uctx);
    const char *func_name;
    for (int i=0; (func_name = hpy_trace_get_func_name(i)); i++)
    {
        /* skip empty names; those indices denote a context handle */
        if (!is_empty(func_name))
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
    HPy_Close(uctx, res);
    HPyTracker_Close(uctx, ht);
    return HPy_NULL;
}

static int check_and_set_func(HPyContext *uctx, HPy arg, HPy *out)
{
    if (HPy_IsNull(arg)) {
        // not provided -> do not change value
        return 0;
    } else if (HPy_Is(uctx, arg, uctx->h_None)) {
        // None -> clear function
        *out = HPy_NULL;
        return 0;
    } else if (!HPyCallable_Check(uctx, arg)) {
        // not null, not None, not callable -> error
        HPyErr_SetString(uctx, uctx->h_TypeError, "Expected a callable object or None");
        return -1;
    }
    // a callable -> set function
    *out = HPy_Dup(uctx, arg);
    return 0;
}

HPyDef_METH(set_trace_functions, "set_trace_functions", set_trace_funcs_impl,
            HPyFunc_KEYWORDS, .doc=
            "Set the functions to call if an HPy API is entered/exited.")
static HPy set_trace_funcs_impl(HPyContext *uctx, HPy self, HPy *args,
        HPy_ssize_t nargs, HPy kw)
{
    HPy h_on_enter = HPy_NULL;
    HPy h_on_exit = HPy_NULL;
    HPyContext *dctx = hpy_trace_get_ctx(uctx);
    HPyTraceInfo *info = get_info(dctx);
    HPyTracker ht;

    static const char *kwlist[] = { "on_enter", "on_exit", NULL };
    if (!HPyArg_ParseKeywords(uctx, &ht, args, nargs, kw, "|OO", kwlist,
            &h_on_enter, &h_on_exit)) {
        return HPy_NULL;
    }

    int r = check_and_set_func(uctx, h_on_enter, &info->on_enter_func) < 0 ||
            check_and_set_func(uctx, h_on_exit, &info->on_exit_func) < 0;
    HPyTracker_Close(uctx, ht);
    if (r) {
        return HPy_NULL;
    }
    return HPy_Dup(uctx, uctx->h_None);
}


/* ~~~~~~ definition of the module hpy.trace._trace ~~~~~~~ */

static HPyDef *module_defines[] = {
    &get_durations,
    &get_call_counts,
    &set_trace_functions,
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
