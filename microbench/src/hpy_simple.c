#include "hpy.h"

/* module-level functions */

HPyDef_METH(noargs, "noargs", noargs_impl, HPyFunc_NOARGS)
static HPy noargs_impl(HPyContext ctx, HPy self)
{
    return HPy_Dup(ctx, ctx->h_None);
}

HPyDef_METH(onearg, "onearg", onearg_impl, HPyFunc_O)
static HPy onearg_impl(HPyContext ctx, HPy self, HPy arg)
{
    return HPy_Dup(ctx, ctx->h_None);
}

HPyDef_METH(varargs, "varargs", varargs_impl, HPyFunc_VARARGS)
static HPy varargs_impl(HPyContext ctx, HPy self, HPy *args, HPy_ssize_t nargs)
{
    return HPy_Dup(ctx, ctx->h_None);
}

HPyDef_METH(allocate_int, "allocate_int", allocate_int_impl, HPyFunc_NOARGS)
static HPy allocate_int_impl(HPyContext ctx, HPy self)
{
    return HPyLong_FromLong(ctx, 2048);
}

HPyDef_METH(allocate_tuple, "allocate_tuple", allocate_tuple_impl, HPyFunc_NOARGS)
static HPy allocate_tuple_impl(HPyContext ctx, HPy self)
{
    //return Py_BuildValue("ii", 2048, 2049);
    HPyErr_SetString(ctx, ctx->h_Exception, "HPy_BuildValue not implemented yet");
    return HPy_NULL;
}


/* Foo type */

typedef struct {
    HPyObject_HEAD
} FooObject;

HPyDef_SLOT(Foo_getitem, Foo_getitem_impl, HPy_sq_item)
static HPy Foo_getitem_impl(HPyContext ctx, HPy self, HPy_ssize_t i)
{
    return HPy_Dup(ctx, ctx->h_None);
}

HPyDef_SLOT(Foo_len, Foo_len_impl, HPy_sq_length)
static HPy_ssize_t Foo_len_impl(HPyContext ctx, HPy self)
{
    return 42;
}


// note that we can reuse the same HPyDef for both module-level and type-level
// methods
static HPyDef *foo_defines[] = {
    &noargs,
    &onearg,
    &varargs,
    &allocate_int,
    &allocate_tuple,
    &Foo_getitem,
    &Foo_len,
};


static HPyType_Spec Foo_spec = {
    .name = "hpy_simple.Foo",
    .basicsize = sizeof(FooObject),
    .flags = HPy_TPFLAGS_DEFAULT,
    .defines = foo_defines
};


/* Module defines */

static HPyDef *module_defines[] = {
    &noargs,
    &onearg,
    &varargs,
    &allocate_int,
    &allocate_tuple,
    NULL
};


static HPyModuleDef moduledef = {
    HPyModuleDef_HEAD_INIT,
    .m_name = "hpy_simple",
    .m_doc = "HPy microbenchmarks",
    .m_size = -1,
    .defines = module_defines,
};

HPy_MODINIT(hpy_simple)
static HPy init_hpy_simple_impl(HPyContext ctx)
{
    HPy m = HPyModule_Create(ctx, &moduledef);
    if (HPy_IsNull(m))
        return HPy_NULL;
    HPy h_Foo = HPyType_FromSpec(ctx, &Foo_spec, NULL);
    if (HPy_IsNull(h_Foo))
      return HPy_NULL;
    HPy_SetAttr_s(ctx, m, "Foo", h_Foo);
    return m;
}
