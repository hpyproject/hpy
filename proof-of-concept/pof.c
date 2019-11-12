#include "hpy.h"

HPy_FUNCTION(do_nothing)
static HPy do_nothing_impl(HPyContext ctx, HPy self, HPy args)
{
    HPy_RETURN_NONE;
}


static HPyMethodDef PofMethods[] = {
    {"do_nothing", do_nothing, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "pof",
    .m_doc = "HPy Proof of Concept",
    .m_size = -1,
    .m_methods = PofMethods
};


static HPy _initpof(HPyContext ctx)
{
    HPy m;
    m = HPyModule_Create(ctx, &moduledef);
    if (m == NULL)
        return NULL;
    return m;
}
HPy_DECLARE_MODINIT(pof, _initpof)
