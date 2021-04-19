#include "hpy.h"

HPyDef_METH(hello, "hello", hello_impl, HPyFunc_NOARGS)
static HPy hello_impl(HPyContext *ctx, HPy self)
{
    return HPyUnicode_FromString(ctx, "hello from pofpackage.foo");
}

static HPyDef *module_defines[] = {
    &hello,
    NULL
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "foo",
    .m_doc = "HPy Proof of Concept",
    .defines = module_defines
};

HPy_MODINIT(foo)
static HPy init_foo_impl(HPyContext *ctx)
{
    return HPyModuleDef_Init(ctx, &moduledef);
}
