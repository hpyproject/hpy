/* Simple module that defines simple functions "myabs" and "add_ints" with
 * varargs calling convention */
/* NOTE: snippets from the following code are referenced from the docs! */

#include "hpy.h"

// This is here to make the module look like an incremental change to simple-example
HPyDef_METH(myabs, "myabs", myabs_impl, HPyFunc_O)
static HPy myabs_impl(HPyContext *ctx, HPy self, HPy arg)
{
    return HPy_Absolute(ctx, arg);
}

// BEGIN: add_ints
HPyDef_METH(add_ints, "add_ints", add_ints_impl, HPyFunc_VARARGS)
static HPy add_ints_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
{
    long a, b;
    if (!HPyArg_Parse(ctx, NULL, args, nargs, "ll", &a, &b))
        return HPy_NULL;
    return HPyLong_FromLong(ctx, a+b);
}
// END: add_ints

// BEGIN: methodsdef
static HPyDef *SimpleMethods[] = {
        &myabs,
        &add_ints,
        NULL,
};
// END: methodsdef

static HPyModuleDef simple = {
        HPyModuleDef_HEAD_INIT,
        .m_name = "hpyvarargs",
        .m_doc = "HPy Example of varargs calling convention",
        .m_size = -1,
        .defines = SimpleMethods
};

HPy_MODINIT(hpyvarargs)
HPy init_hpyvarargs_impl(HPyContext *ctx) {
    return HPyModule_Create(ctx, &simple);
}
