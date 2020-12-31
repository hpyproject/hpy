// Python-level interface for the _debug module. Written in HPy itself, the
// idea is that it should be reusable by other implementations

#include "hpy.h"

HPyDef_METH(hello, "hello", hello_impl, HPyFunc_NOARGS)
static HPy hello_impl(HPyContext ctx, HPy self)
{
    return HPyUnicode_FromString(ctx, "hello world");
}

static HPyDef *module_defines[] = {
    &hello,
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
