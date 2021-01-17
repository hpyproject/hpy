// Python-level interface for the _debug module. Written in HPy itself, the
// idea is that it should be reusable by other implementations

// NOTE: hpy.debug._debug is loaded using the UNIVERSAL ctx. To make it
// clearer, we will use "uctx" and "dctx" to distinguish them.

#include "hpy.h"
#include "debug_internal.h"

HPyDef_METH(_get_open_handles, "_get_open_handles", _get_open_handles_impl, HPyFunc_NOARGS)
static HPy _get_open_handles_impl(HPyContext uctx, HPy self)
{
    HPyContext dctx = hpy_debug_get_ctx(uctx);
    HPyDebugInfo *info = get_info(dctx);

    UHPy u_result = HPyList_New(uctx, 0);
    if (HPy_IsNull(u_result))
        return HPy_NULL;

    DebugHandle *dh = info->open_handles;
    while(dh != NULL) {
        if (HPyList_Append(uctx, u_result, dh->uh) == -1) {
            HPy_Close(uctx, u_result);
            return HPy_NULL;
        }
        dh = dh->next;
    }
    return u_result;
}

static HPyDef *module_defines[] = {
    &_get_open_handles,
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
