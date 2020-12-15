#include "_ctx.h"
#include <stdio.h>
#include <string.h>


static HPy dbg_Add(HPyContext ctx, HPy a, HPy b)
{
    printf("dbg_Add...\n");
    HPyDebugInfo *info = get_info(ctx);
    return HPy_Add(info->original_ctx, a, b);
}

HPyDef_METH(get_debug_ctx, "get_debug_ctx", get_debug_ctx_impl, HPyFunc_NOARGS)
static HPy get_debug_ctx_impl(HPyContext ctx, HPy self)
{
    // XXX: these mallocs are never freed

    // initialize info
    HPyDebugInfo *info = malloc(sizeof(HPyDebugInfo));
    info->magic_number = HPY_DEBUG_MAGIC;
    info->original_ctx = ctx;

    // initialize ctx2
    HPyContext ctx2 = malloc(sizeof(struct _HPyContext_s));
    memcpy(ctx2, ctx, sizeof(struct _HPyContext_s));
    ctx2->name = "HPy Debug Mode";
    ctx2->_private = info;
    ctx2->ctx_Add = dbg_Add;
    return HPyLong_FromLong(ctx, (long)ctx2);
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
    return m;
}
