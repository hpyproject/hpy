// Python-level interface for the _debug module. Written in HPy itself, the
// idea is that it should be reusable by other implementations

// NOTE: hpy.debug._debug is loaded using the UNIVERSAL ctx. To make it
// clearer, we will use "uctx" and "dctx" to distinguish them.

#include "hpy.h"
#include "debug_internal.h"

HPyDef_METH(new_generation, "new_generation", new_generation_impl, HPyFunc_NOARGS)
static HPy new_generation_impl(HPyContext uctx, HPy self)
{
    HPyContext dctx = hpy_debug_get_ctx(uctx);
    HPyDebugInfo *info = get_info(dctx);
    info->current_generation++;
    return HPyLong_FromLong(uctx, info->current_generation);
}


// TODO: eventually, we want to return Python-level views of DebugHandle, so
// that we can retrieve additional infos from applevel (e.g., the C backtrace
// at the moment of opening or so). For now, just return the Python objects
// pointed by the handles.
HPyDef_METH(get_open_handles, "get_open_handles", get_open_handles_impl, HPyFunc_O, .doc=
            "Return a list containing all the open handles whose generation is >= "
            "of the given arg")
static HPy get_open_handles_impl(HPyContext uctx, UHPy u_self, UHPy u_gen)
{
    HPyContext dctx = hpy_debug_get_ctx(uctx);
    HPyDebugInfo *info = get_info(dctx);

    long gen = HPyLong_AsLong(uctx, u_gen);
    if (HPyErr_Occurred(uctx))
        return HPy_NULL;

    UHPy u_result = HPyList_New(uctx, 0);
    if (HPy_IsNull(u_result))
        return HPy_NULL;

    DebugHandle *dh = info->open_handles;
    while(dh != NULL) {
        if (dh->generation >= gen) {
            if (HPyList_Append(uctx, u_result, dh->uh) == -1) {
                HPy_Close(uctx, u_result);
                return HPy_NULL;
            }
        }
        dh = dh->next;
    }
    return u_result;
}

static HPyDef *module_defines[] = {
    &new_generation,
    &get_open_handles,
    NULL
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "hpy.debug._debug",
    .m_doc = "HPy debug mode",
    .m_size = -1,
    .defines = module_defines
};


HPy_MODINIT(_debug)
static HPy init__debug_impl(HPyContext ctx)
{
    HPy m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    return m;
}
