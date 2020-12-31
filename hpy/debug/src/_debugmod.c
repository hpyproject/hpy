// Python-level interface for the _debug module. Written in HPy itself, the
// idea is that it should be reusable by other implementations

#include "hpy.h"
#include "debug_internal.h"

/* WARNING: all the functions called _test_* are used only for testing. They
   are not supposed to be used outside tests */

HPyDef_METH(_test_DHPy_new, "_test_DHPy_new", _test_DHPy_new_impl, HPyFunc_O)
static HPy _test_DHPy_new_impl(HPyContext ctx, HPy self, HPy arg)
{
    HPyContext debug_ctx = hpy_debug_get_ctx(ctx);
    HPy h2 = HPy_Dup(ctx, arg);
    DHPy dh = DHPy_new(debug_ctx, h2);
    // return the numeric value of the pointer, although it's a bit useless
    return HPyLong_FromSsize_t(ctx, (HPy_ssize_t)dh);
}

HPyDef_METH(_test_get_open_handles, "_test_get_open_handles", _test_get_open_handles_impl, HPyFunc_NOARGS)
static HPy _test_get_open_handles_impl(HPyContext ctx, HPy self)
{
    HPyContext debug_ctx = hpy_debug_get_ctx(ctx);
    HPyDebugInfo *info = get_info(debug_ctx);
    HPy hlist = HPyList_New(ctx, 0);
    DHPy dh = info->open_handles;
    while (dh != NULL) {
        HPyList_Append(ctx, hlist, dh->h);
        dh = dh->next;
    }
    return hlist;
}

static HPyDef *module_defines[] = {
    &_test_DHPy_new,
    &_test_get_open_handles,
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
