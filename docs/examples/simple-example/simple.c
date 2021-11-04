/* Simple C module that defines single simple function "myabs".
 * We need to have a separate standalone package for those snippets, because we
 * want to show the source code in its entirety, including the HPyDef array
 * initialization, the module definition, and the setup.py script, so there is
 * no room left for mixing these code snippets with other code snippets.
 */

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
        .defines = SimpleMethods,
        .legacy_methods = NULL
};
// END: methodsdef

// BEGIN: moduledef
HPy_MODINIT(simple)
HPy init_simple_impl(HPyContext *ctx) {
    return HPyModule_Create(ctx, &simple);
}
// END: moduledef