/* Simple module that defines single simple function "myabs" */
/* NOTE: snippets from the following code are referenced from the docs! */

// For the following two illustrative snippets we just check that they compile:
#include "hpy.h"

// BEGIN: foo
void foo(HPyContext *ctx)
{
    HPy x = HPyLong_FromLong(ctx, 42);
    HPy y = HPy_Dup(ctx, x);
    /* ... */
    // we need to close x and y independently
    HPy_Close(ctx, x);
    HPy_Close(ctx, y);
}
// END: foo

// BEGIN: is_same_object
int is_same_object(HPyContext *ctx, HPy x, HPy y)
{
    // return x == y; // compilation error!
    return HPy_Is(ctx, x, y);
}
// END: is_same_object

// -------------

// BEGIN: myabs
#include "hpy.h"

HPyDef_METH(myabs, "myabs", myabs_impl, HPyFunc_O)
static HPy myabs_impl(HPyContext *ctx, HPy self, HPy arg)
{
    return HPy_Absolute(ctx, arg);
}
// END: myabs

// BEGIN: methodsdef
static HPyDef *SimpleMethods[] = {
        &myabs,
        NULL,
};

static HPyModuleDef simple = {
        HPyModuleDef_HEAD_INIT,
        .m_name = "simple",
        .m_doc = "HPy Example",
        .m_size = -1,
        .defines = SimpleMethods
};
// END: methodsdef

// BEGIN: moduledef
HPy_MODINIT(simple)
HPy init_simple_impl(HPyContext *ctx) {
    return HPyModule_Create(ctx, &simple);
}
// END: moduledef