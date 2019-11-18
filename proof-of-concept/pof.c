#include "hpy.h"

HPy_METH_NOARGS(do_nothing)
static HPy do_nothing_impl(HPyContext ctx, HPy self)
{
    return HPyNone_Get(ctx);
}

HPy_METH_O(double_obj)
static HPy double_obj_impl(HPyContext ctx, HPy self, HPy obj)
{
    return HPyNumber_Add(ctx, obj, obj);
}

HPy_METH_VARARGS(add_ints)
static HPy add_ints_impl(HPyContext ctx, HPy self, HPy *args, Py_ssize_t nargs)
{
    long a, b;
    if (!HPyArg_Parse(ctx, args, nargs, "ll", &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}


static HPyMethodDef PofMethods[] = {
    {"do_nothing", do_nothing, METH_NOARGS, ""},
    {"double", double_obj, METH_O, ""},
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
static HPy init_pof_impl(HPyContext ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    return m;
}
