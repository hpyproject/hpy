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
    .name = "foo",
    .doc = "HPy Proof of Concept",
    .size = -1,
    .defines = module_defines
};

HPy_MODINIT(foo)
static HPy init_foo_impl(HPyContext *ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    return m;
}
