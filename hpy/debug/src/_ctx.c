#include "_ctx.h"
#include <stdio.h>
#include <string.h>

static HPyDebugInfo debug_info = {
    .magic_number = HPY_DEBUG_MAGIC,
    .original_ctx = NULL,
};

static struct _HPyContext_s debug_ctx = {
    .name = NULL,
};


static HPy dbg_Add(HPyContext ctx, HPy a, HPy b)
{
    printf("dbg_Add...\n");
    HPyDebugInfo *info = get_info(ctx);
    return HPy_Add(info->original_ctx, a, b);
}

void debug_ctx_init(HPyContext original_ctx)
{
    if (debug_ctx.name)
        return; // already initialized

    // initialize debug_info
    debug_info.original_ctx = original_ctx;

    // initialize debug_ctx: eventually we will autogen a static initializer
    // for debug_ctx. For now, just copy&fix
    memcpy(&debug_ctx, original_ctx, sizeof(struct _HPyContext_s));
    debug_ctx.name = "HPy Debug Mode ABI";
    debug_ctx._private = &debug_info;
    debug_ctx.ctx_Add = dbg_Add;
}


HPyDef_METH(get_debug_ctx, "get_debug_ctx", get_debug_ctx_impl, HPyFunc_NOARGS)
static HPy get_debug_ctx_impl(HPyContext ctx, HPy self)
{
    assert(debug_ctx.name != NULL);
    return HPyLong_FromLong(ctx, (long)&debug_ctx);
}


static HPyDef *module_defines[] = {
    &get_debug_ctx,
    NULL
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "hpy.debug._ctx",
    .m_doc = "HPy debug context",
    .m_size = -1,
    .defines = module_defines
};


HPy_MODINIT(_ctx)
static HPy init__ctx_impl(HPyContext ctx)
{
    HPy m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    debug_ctx_init(ctx);
    return m;
}
