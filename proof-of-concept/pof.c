#include "hpy.h"

HPy_FUNCTION(do_nothing)
static HPy do_nothing_impl(HPyContext ctx, HPy self, HPy args)
{
    HPy_RETURN_NONE;
}

HPy_FUNCTION(add_ints)
static HPy add_ints_impl(HPyContext ctx, HPy self, HPy args)
{
    long a, b;
    if (!HPyArg_ParseTuple(ctx, args, "ll", &a, &b))
        return NULL;
    return HPyLong_FromLong(ctx, a+b);
}


static HPyMethodDef PofMethods[] = {
    {"do_nothing", do_nothing, METH_NOARGS, ""},
    {"add_ints", add_ints, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "pof",
    .m_doc = "HPy Proof of Concept",
    .m_size = -1,
    .m_methods = PofMethods
};


HPy_MODINIT(pof)
static HPy HPyInit_pof(HPyContext ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (m == NULL)
        return NULL;
    return m;
}
