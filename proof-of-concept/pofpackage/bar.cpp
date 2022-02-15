#include "hpy.h"

class Bar
{
    int foo;
    public:
        Bar(int f)
        {
            foo = f;
        }

        int boo(HPyContext *ctx, HPy obj)
        {
            return foo + HPyLong_AsLong(ctx, obj);
        }
};

HPyDef_METH(hello, "hello", hello_impl, HPyFunc_O)
static HPy hello_impl(HPyContext *ctx, HPy self, HPy obj)
{
    Bar b(21);
    return HPyLong_FromLong(ctx, b.boo(ctx, obj));
}


static HPyDef *module_defines[] = {
    &hello,
    NULL
};
static HPyModuleDef moduledef = {
    .name = "bar",
    .doc = "HPy C++ Proof of Concept",
    .size = -1,
    .defines = module_defines
};

#ifdef __cplusplus
extern "C" {
#endif


HPy_MODINIT(bar)
static HPy init_bar_impl(HPyContext *ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    return m;
}

#ifdef __cplusplus
}
#endif
