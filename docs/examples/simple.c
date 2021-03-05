#include "hpy.h"

HPy_DEF_METH_O(myabs)
static HPy myabs_impl(HPyContext *ctx, HPy self, HPy obj)
{
    return HPy_Absolute(ctx, obj);
}

HPy_DEF_METH_VARARGS(add_ints)
static HPy add_ints_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
{
    long a, b;
    if (!HPyArg_Parse(ctx, args, nargs, "ll", &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}

static HPyMethodDef SimpleMethods[] = {
    {"myabs", myabs, HPy_METH_O, "Compute the absolute value of the given argument"},
    {"add_ints", add_ints, HPy_METH_VARARGS, "Add two integers"},
    {NULL, NULL, 0, NULL}
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "simple",
    .m_doc = "HPy Example",
    .m_size = -1,
    .m_methods = SimpleMethods
};


HPy_MODINIT(simple)
static HPy init_simple_impl(HPyContext *ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    return m;
}
